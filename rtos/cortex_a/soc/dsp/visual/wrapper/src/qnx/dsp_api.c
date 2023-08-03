/**
 *  @file dsp_api.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details DSP APIs
 *
 */


#include "dsp_wrapper_osal.h"
#include "AmbaDSP.h"
#include "ambadsp_ioctl.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaDSP_StillCapture.h"
#include "AmbaDSP_VideoDec.h"
#include "AmbaDSP_StillDec.h"
#include <AmbaVIN.h>
#include <AmbaVOUT.h>
//#include <hw/ambarella_vin.h>
#include "AmbaSYS.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageDefine.h"
#include "AmbaDSP_ImageUtility.h"
static UINT8 DspOpMode = AMBA_DSP_SYS_STATE_UNKNOWN;

static UINT32 ViewZoneIkCtxId[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
static UINT8  ViewZonePipe[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
static UINT8  ViewZoneRotate[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
static UINT16 ViewZoneSource[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
#else
extern UINT32 AmbaIK_GetIsoHdrRawOffest(const void *pIsocfg, AMBA_IK_HDR_RAW_INFO_s *pHdrRawInfo);
#endif

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
static UINT32 VoutDisplayOption[AMBA_DSP_MAX_VOUT_NUM] = {0};
static UINT16 ViewZoneMaxHorWarpComp[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
#endif

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
static AMBA_DSP_LIVEVIEW_SLICE_CFG_s ViewZoneSliceCfg[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
static UINT32 IsLvTileMode = 0U;
#endif

static dsp_liveview_update_pymdcfg_t DspLiveviewUpdatePymdcfg;
typedef struct {
    ULONG  Addr;
    UINT32 Size;
    UINT32 IsCached;
    UINT32 PhysAddr;
} dsp_buf_pool_t;
static dsp_buf_pool_t DspBufAddr[NUM_DSP_WRAP_POOL] = {0UL};

static inline int32_t DSP_GET_FD(void)
{
    static int32_t ambadsp_fd = 0;
    int32_t Rval = 0;

    /* Open */
    if (ambadsp_fd == 0) {
        Rval = open("/dev/ambadsp", O_RDWR);
        if (Rval > 0) {
            ambadsp_fd = Rval;

#if 0
            dsp_wrapper_osal_printU5("============= DSP API Sizes =============", 0U, 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("[MAIN]", 0U, 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  dsp_liveview_resc_t               = %d", sizeof(dsp_liveview_resc_t), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  dsp_resc_limit_cfg_t              = %d", sizeof(dsp_resc_limit_cfg_t), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("[EVNT]", 0U, 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  dsp_evnt_data_t                   = %d", sizeof(dsp_evnt_data_t), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_DSP_RAW_DATA_RDY_s(EXT?)     = %d", sizeof(AMBA_DSP_RAW_DATA_RDY_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  dsp_stat_buf_t                    = %d", sizeof(dsp_stat_buf_t), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_DSP_YUV_DATA_RDY_s           = %d", sizeof(AMBA_DSP_YUV_DATA_RDY_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_DSP_YUV_DATA_RDY_EXTEND_s    = %d", sizeof(AMBA_DSP_YUV_DATA_RDY_EXTEND_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_DSP_PYMD_DATA_RDY_s          = %d", sizeof(AMBA_DSP_PYMD_DATA_RDY_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_DSP_VOUT_DATA_INFO_s         = %d", sizeof(AMBA_DSP_VOUT_DATA_INFO_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_DSP_ENC_PIC_RDY_s            = %d", sizeof(AMBA_DSP_ENC_PIC_RDY_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_DSP_EVENT_VIDEO_ENC_STATUS_s = %d", sizeof(AMBA_DSP_EVENT_VIDEO_ENC_STATUS_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_DSP_STL_RAW_DATA_INFO_s      = %d", sizeof(AMBA_DSP_STL_RAW_DATA_INFO_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_DSP_YUV_IMG_BUF_s            = %d", sizeof(AMBA_DSP_YUV_IMG_BUF_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_DSP_VIDEO_PATH_INFO_s        = %d", sizeof(AMBA_DSP_VIDEO_PATH_INFO_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_DSP_EVENT_VIDEO_DEC_STATUS_UPDATE_s = %d", sizeof(AMBA_DSP_EVENT_VIDEO_DEC_STATUS_UPDATE_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_DSP_VIDEO_DEC_YUV_DISP_REPORT_s = %d", sizeof(AMBA_DSP_VIDEO_DEC_YUV_DISP_REPORT_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_DSP_EVENT_VIDDEC_PIC_EX_s    = %d", sizeof(AMBA_DSP_EVENT_VIDDEC_PIC_EX_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_DSP_STLDEC_STATUS_s          = %d", sizeof(AMBA_DSP_STLDEC_STATUS_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_DSP_STILL_DEC_YUV_DISP_REPORT_s = %d", sizeof(AMBA_DSP_STILL_DEC_YUV_DISP_REPORT_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_DSP_VIN_POST_CONFIG_STATE_s          = %d", sizeof(AMBA_DSP_VIN_POST_CONFIG_STATE_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_DSP_LV_SYNC_JOB_INFO_s       = %d", sizeof(AMBA_DSP_LV_SYNC_JOB_INFO_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_DSP_ENC_MV_RDY_s             = %d", sizeof(AMBA_DSP_ENC_MV_RDY_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_DSP_SIDEBAND_DATA_RDY_s      = %d", sizeof(AMBA_DSP_SIDEBAND_DATA_RDY_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_DSP_LNDT_DATA_RDY_s          = %d", sizeof(AMBA_DSP_LNDT_DATA_RDY_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_IK_CFA_3A_DATA_s             = %d", sizeof(AMBA_IK_CFA_3A_DATA_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_IK_PG_3A_DATA_s              = %d", sizeof(AMBA_IK_PG_3A_DATA_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  AMBA_IK_CFA_HIST_STAT_s           = %d", sizeof(AMBA_IK_CFA_HIST_STAT_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("[LV]", 0U, 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  dsp_liveview_yuv_buf_t            = %d", sizeof(dsp_liveview_yuv_buf_t), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  dsp_liveview_viewzone_cfg_t       = %d", sizeof(dsp_liveview_viewzone_cfg_t), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  dsp_liveview_strm_cfg_t           = %d", sizeof(dsp_liveview_strm_cfg_t), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  dsp_liveview_cfg_t                = %d", sizeof(dsp_liveview_cfg_t), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  dsp_liveview_update_cfg_t         = %d", sizeof(dsp_liveview_update_cfg_t), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("[VENC]", 0U, 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  dsp_video_enc_cfg_t               = %d", sizeof(dsp_video_enc_cfg_t), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("[VIN]", 0U, 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  dsp_vin_drv_cfg_cfa_t             = %d", sizeof(dsp_vin_drv_cfg_cfa_t), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("[IK]", 0U, 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  dsp_ik_drv_cfg_sensor_info_t      = %d", sizeof(dsp_ik_drv_cfg_sensor_info_t), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  dsp_ik_drv_cfg_ability_t          = %d", sizeof(dsp_ik_drv_cfg_ability_t), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  dsp_ik_drv_cfg_iso_t              = %d", sizeof(dsp_ik_drv_cfg_iso_t), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  dsp_ik_drv_cfg_dol_offset_t       = %d", sizeof(dsp_ik_drv_cfg_dol_offset_t), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("  IDSP_INFO_s                       = %d", sizeof(IDSP_INFO_s), 0U, 0U, 0U, 0U);
            dsp_wrapper_osal_printU5("=========================================", 0U, 0U, 0U, 0U, 0U);
#endif
        }
    } else {
        Rval = ambadsp_fd;
    }

    return Rval;
}

static uint32_t dsp_wrap_get_ioctl_rval(int32_t retcode, const char *fn)
{
    uint32_t rval;

    if (retcode < 0) {
        rval = DSP_ERR_0003;
        dsp_wrapper_osal_printS5("[ERR] %s : ioctl fail", fn, NULL, NULL, NULL, NULL);
        dsp_wrapper_osal_printI5("           errno = 0x%x", errno, 0, 0, 0, 0);
    } else {
        rval = (uint32_t)retcode;
    }
    return rval;
}

static inline uint8_t IS_SUCCESSOR_STATE(void)
{
    uint8_t Bool = 0U;
    uint32_t Rval = DSP_ERR_NONE;
    UINT32 Data0 = 0U, Data1 = 0U;

    if (DspOpMode == AMBA_DSP_SYS_STATE_UNKNOWN) {
        Rval = AmbaDSP_GetStatus(0U, 0U, &Data0, &Data1);
        if (Rval != DSP_ERR_NONE) {
            dsp_wrapper_osal_printU5("[ERR] IS_SUCCESSOR_STATE [0x%X]", Rval, 0U, 0U, 0U, 0U);
        } else {
            if (Data0 != AMBA_DSP_SYS_STATE_UNKNOWN) {
                Bool = 1U;
            } else {
                Bool = 0U;
            }
        }
    }

    return Bool;
}

/************ Event ********************/
#define WAIT_DSP_EVENT_MUTEX_TIMEOUT_10S    (10000U)
#ifndef DSP_WRAP_EVENT_GET_DATA_NO_THREAD
#define DSP_WRAPPER_MSG_PARSE_START         (0x00000001U)
#define DSP_WRAPPER_MSG_PARSE_END           (0x00000002U)
#define DSP_WRAPPER_MSG_PARSE_EVNT_TIMEOUT  (1000U)
#endif

static DSP_WRAP_EVENT_HANDLER_CTRL_s DspWrapEventHandler[AMBA_DSP_EVENT_NUM];
#ifndef DSP_WRAP_EVENT_GET_DATA_NO_THREAD
static wrap_osal_thread_t DspWrapperEventDataTask;
#define DSP_WRAPPER_EVNT_TASK_CORE_MAP  (0x1U)
#define DSP_WRAPPER_EVNT_TASK_PRIORITY  (10U)
#define DSP_WRAPPER_EVNT_TASK_SIZE      (0x4000U)
static UINT8 DspWrapperEventDataTaskBuf[DSP_WRAPPER_EVNT_TASK_SIZE];
static wrap_osal_even_t DspWrapperMsgParseEvent;
void* dsp_wrap_event_data_task(void *info)
{
    uint32_t Rval;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_evnt_data_t DspEventData = {0};
    uint32_t while_one = 1U;
    uint32_t get_data_loop = 1U;
    uint32_t actual_flag = 0U;
    uint32_t req_flag = DSP_WRAPPER_MSG_PARSE_START | DSP_WRAPPER_MSG_PARSE_END;

//FIXME, Misra
(void)info;
    dsp_wrapper_osal_printU5("dsp_wrap_event_data_task Start", 0U, 0U, 0U, 0U, 0U);

    while(while_one > 0) {
        Rval = dsp_wrapper_osal_eventflag_get(&DspWrapperMsgParseEvent, req_flag, 0U, 0U, &actual_flag, DSP_WRAPPER_MSG_PARSE_EVNT_TIMEOUT);
        if (Rval != DSP_ERR_NONE) {
            if (Rval == DSP_ERR_TIMEOUT) {
                actual_flag = 0U;
            } else {
                dsp_wrapper_osal_printU5("[ERR] dsp_wrap_event_data_task : WaitEvnt failed [0x%x]", Rval, 0U, 0U, 0U, 0U);
            }
        }

        get_data_loop = 1U;
        while(get_data_loop > 0) {
            /* ioctl */
            if (dsp_fd > 0) {
                retcode = ioctl(dsp_fd, AMBADSP_EVENT_GET_DATA, &DspEventData);
                Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
                if (Rval == DSP_ERR_NONE) {
                    if (DspEventData.EvntDataValid == 1U) {
                        Rval = DspWrapGiveEvent(DspEventData.EventId, &DspEventData.EventData);
                    }
                    if (DspEventData.PoolEmpty == 1U) {
                        if ((actual_flag & req_flag) == req_flag) {
                            dsp_wrapper_osal_eventflag_clear(&DspWrapperMsgParseEvent, req_flag);
                        } else {
                            Rval = dsp_wrapper_osal_sleep(1U);
                            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                        }
                        get_data_loop = 0U;
                    }
                } else {
                    get_data_loop = 0U;
                }
            }
        }
    }
}
#endif

static uint32_t DspWrapEventInit(void)
{
    uint32_t Rval = DSP_ERR_NONE;
    uint32_t i;
    static uint32_t DspWrapEventInitFlag = 0U;
    static char DspWrapEvntMtxName[] = "DspWrapEvntMtx";
#ifndef DSP_WRAP_EVENT_GET_DATA_NO_THREAD
    static char DspWrapEvntQueryTask[] = "DspWrapEvntQueryTask";
    static char DspWrapEvntFlagName[] = "DspWrapEvntFlag";
    uint8_t *pU8;
    void *pVoid;
#endif

    if (DspWrapEventInitFlag == 0U) {
        Rval = dsp_wrapper_osal_memset(DspWrapEventHandler, 0, sizeof(DspWrapEventHandler));
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

        for (i = 0U; i < AMBA_DSP_EVENT_NUM; i++) {
            Rval = dsp_wrapper_osal_mutex_init(&(DspWrapEventHandler[i].Mtx), DspWrapEvntMtxName);
            if (Rval != DSP_ERR_NONE) {
                dsp_wrapper_osal_printU5("[ERR] DspWrapEventInit : Create Event mutex fail", 0U, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
                break;
            }
        }
#ifndef DSP_WRAP_EVENT_GET_DATA_NO_THREAD
        if (Rval == DSP_ERR_NONE) {
            Rval = dsp_wrapper_osal_eventflag_init(&DspWrapperMsgParseEvent, DspWrapEvntFlagName);
            if (Rval == DSP_ERR_NONE) {
                Rval = dsp_wrapper_osal_eventflag_clear(&DspWrapperMsgParseEvent, DSP_WRAPPER_MSG_PARSE_START|DSP_WRAPPER_MSG_PARSE_END);
            }
        }
        if (Rval == DSP_ERR_NONE) {
            pU8 = DspWrapperEventDataTaskBuf;
            dsp_wrapper_osal_typecast(&pVoid, &pU8);
            Rval = dsp_wrapper_osal_thread_create(&DspWrapperEventDataTask, DspWrapEvntQueryTask, DSP_WRAPPER_EVNT_TASK_PRIORITY,
                                                  dsp_wrap_event_data_task, NULL, pVoid,
                                                  DSP_WRAPPER_EVNT_TASK_SIZE, 0U/*AutoStart*/);

            if (Rval != DSP_ERR_NONE) {
                dsp_wrapper_osal_printU5("[ERR] DspWrapEventInit : DspWrapEvntQueryTask Create fail 0x%X", Rval, 0, 0, 0, 0);
                Rval = DSP_ERR_0003;
            }
            if (Rval == DSP_ERR_NONE) {
                Rval = dsp_wrapper_osal_thread_set_affinity(&DspWrapperEventDataTask, DSP_WRAPPER_EVNT_TASK_CORE_MAP);
                if (Rval != DSP_ERR_NONE) {
                    dsp_wrapper_osal_printU5("[ERR] DspWrapEventInit : DspWrapEvntQueryTask CoreSelection 0x%X", Rval, 0, 0, 0, 0);
                    Rval = ERR_ARG;
                }
            }
            if (Rval == DSP_ERR_NONE) {
                Rval = dsp_wrapper_osal_thread_resume(&DspWrapperEventDataTask);
                if (Rval != DSP_ERR_NONE) {
                    dsp_wrapper_osal_printU5("[ERR] DspWrapEventInit : DspWrapEvntQueryTask Resume 0x%X", Rval, 0, 0, 0, 0);
                }
            }
        }
#endif
        if (Rval == DSP_ERR_NONE) {
            DspWrapEventInitFlag = 1U;
        }
    }

    return Rval;
}

static inline uint32_t DspWrapEventTakeMtx(uint16_t EventID, DSP_WRAP_EVENT_HANDLER_CTRL_s **ppEventHdlr)
{
    uint32_t Rval;

    Rval = DspWrapEventInit();

    if (Rval == DSP_ERR_NONE) {
        if (EventID >= AMBA_DSP_EVENT_NUM) {
            dsp_wrapper_osal_printU5("[ERR] DspWrapEventTakeMtx : EvntId[%d]", EventID, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0000;
        }
    }
    if (Rval == DSP_ERR_NONE) {
        if (ppEventHdlr == NULL) {
            Rval = DSP_ERR_0000;
        } else {
            *ppEventHdlr = &DspWrapEventHandler[EventID];
            if (dsp_wrapper_osal_mutex_lock(&(*ppEventHdlr)->Mtx, WAIT_DSP_EVENT_MUTEX_TIMEOUT_10S) != OK) {
                dsp_wrapper_osal_printU5("[ERR] DspWrapEventTakeMtx : Get mtx on Envt[%d]", EventID, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            }
        }
    }

    return Rval;
}

static inline uint32_t DspWrapEventGiveMtx(DSP_WRAP_EVENT_HANDLER_CTRL_s *pEventHdlr)
{
    uint32_t Rval;

    Rval = DspWrapEventInit();

    if (Rval == DSP_ERR_NONE) {
        if (pEventHdlr == NULL) {
            Rval = DSP_ERR_0000;
        } else {
            Rval = dsp_wrapper_osal_mutex_unlock(&pEventHdlr->Mtx);
            if (Rval != DSP_ERR_NONE) {
                dsp_wrapper_osal_printU5("[ERR] DspWrapEventGiveMtx : Release mtx failed", 0U, 0U, 0U, 0U, 0U);
                Rval = DSP_ERR_0003;
            }
        }
   }

    return Rval;
}

uint32_t DspWrapEventHandlerConfig(uint16_t EventID, uint32_t MaxNumHandlers, AMBA_DSP_EVENT_HANDLER_f *pEventHandlers)
{
    uint32_t Rval;
    DSP_WRAP_EVENT_HANDLER_CTRL_s *pEventHdlrCtrl;

    Rval = DspWrapEventInit();

    if (Rval == DSP_ERR_NONE) {
        if (pEventHandlers == NULL) {
            Rval = DSP_ERR_0000;
        } else if (DspWrapEventTakeMtx(EventID, &pEventHdlrCtrl) != OK) {
            Rval = DSP_ERR_0003;
        } else {
            pEventHdlrCtrl->Main.MaxNumHandlers = MaxNumHandlers;
            pEventHdlrCtrl->Main.pEventHandlers = pEventHandlers;
            Rval = DspWrapEventGiveMtx(pEventHdlrCtrl);
        }
    }

    return Rval;
}

uint32_t DspWrapRegisterEventHandler(uint16_t EventID, AMBA_DSP_EVENT_HANDLER_f EventHandler)
{
    uint32_t Rval;
    DSP_WRAP_EVENT_HANDLER_CTRL_s  *pEventHdlrCtrl;
    const DSP_WRAP_EVENT_HANDLER_s *pEventHdlr = NULL;
    AMBA_DSP_EVENT_HANDLER_f  *pEventHdlrFunc;
    uint32_t Idx;
    uint32_t IdxMax = 0;

    Rval = DspWrapEventInit();

    if (Rval == DSP_ERR_NONE) {
        if (DspWrapEventTakeMtx(EventID, &pEventHdlrCtrl) != OK) {
            Rval = DSP_ERR_0003;
        }
    }

    if (Rval == DSP_ERR_NONE) {
        pEventHdlr = &(pEventHdlrCtrl->Main);
        if (pEventHdlr != NULL) {
            IdxMax          = pEventHdlr->MaxNumHandlers;
            pEventHdlrFunc  = pEventHdlr->pEventHandlers;

            for (Idx = 0U; Idx < IdxMax; Idx++) {
                if (pEventHdlrFunc[Idx] == NULL) {
                    pEventHdlrFunc[Idx] = EventHandler;
                    break;
                }
            }
            if (Idx == IdxMax) {
                dsp_wrapper_osal_printU5("[ERR] DspWrapRegisterEventHandler[%d] : No more space[%d], ignored!", EventID, IdxMax, 0U, 0U, 0U);
            }
            Rval = DspWrapEventGiveMtx(pEventHdlrCtrl);
        }
    }

    return Rval;
}

uint32_t DspWrapUnRegisterEventHandler(uint16_t EventID, AMBA_DSP_EVENT_HANDLER_f EventHandler)
{
    uint32_t Rval;
    DSP_WRAP_EVENT_HANDLER_CTRL_s  *pEventHdlrCtrl;
    const DSP_WRAP_EVENT_HANDLER_s *pEventHdlr = NULL;
    AMBA_DSP_EVENT_HANDLER_f  *pEventHdlrFunc;
    uint32_t Idx;
    uint32_t IdxMax = 0;

    Rval = DspWrapEventInit();

    if (Rval == DSP_ERR_NONE) {
        if (DspWrapEventTakeMtx(EventID, &pEventHdlrCtrl) != OK) {
            Rval = DSP_ERR_0003;
        }
    }

    if (Rval == DSP_ERR_NONE) {
        pEventHdlr = &(pEventHdlrCtrl->Main);
        if (pEventHdlr != NULL) {
            IdxMax          = pEventHdlr->MaxNumHandlers;
            pEventHdlrFunc  = pEventHdlr->pEventHandlers;

            for (Idx = 0U; Idx < IdxMax; Idx++) {
                if (pEventHdlrFunc[Idx] == EventHandler) {
                    pEventHdlrFunc[Idx] = NULL;
                    break;
                }
            }
            if (Idx == IdxMax) {
                dsp_wrapper_osal_printU5("[ERR] DspWrapUnRegisterEventHandler[%d] : can't find, ignored!",EventID, 0U, 0U, 0U, 0U);
            }
            Rval = DspWrapEventGiveMtx(pEventHdlrCtrl);
        }
    }

    return Rval;
}

uint32_t DspWrapEventHandlerReset(uint16_t EventID)
{
    uint32_t Rval;
    DSP_WRAP_EVENT_HANDLER_CTRL_s    *pEventHdlrCtrl;
    DSP_WRAP_EVENT_HANDLER_s         *pEventHdlr = NULL;

    Rval = DspWrapEventInit();

    if (Rval == DSP_ERR_NONE) {
        if (DspWrapEventTakeMtx(EventID, &pEventHdlrCtrl) != OK) {
            Rval = DSP_ERR_0003;
        }
    }

    if (Rval == DSP_ERR_NONE) {
        pEventHdlr = &(pEventHdlrCtrl->Main);
        if (pEventHdlr != NULL) {
            Rval = dsp_wrapper_osal_memset(pEventHdlr, 0, sizeof(DSP_WRAP_EVENT_HANDLER_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        }
        Rval = DspWrapEventGiveMtx(pEventHdlrCtrl);
    }
    return Rval;
}

static uint32_t DspEventLvRawRdy(const ULONG InfoULAddr)
{
    AMBA_DSP_RAW_DATA_RDY_s *pRawDataRdy = NULL;
    UINT32 Rval;
    ULONG ULAddr;

    dsp_wrapper_osal_typecast(&pRawDataRdy, &InfoULAddr);

    /* P2V */
    Rval = dsp_wrapper_osal_phys2virt((UINT32)pRawDataRdy->RawBuffer.BaseAddr, &ULAddr);
    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
    pRawDataRdy->RawBuffer.BaseAddr = ULAddr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_wrapper_osal_phys2virt((UINT32)pRawDataRdy->AuxBuffer.BaseAddr, &ULAddr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        pRawDataRdy->AuxBuffer.BaseAddr = ULAddr;
    }

    return Rval;
}

static uint32_t DspEventLvYuvDataRdy(const ULONG InfoULAddr)
{
    AMBA_DSP_YUV_DATA_RDY_EXTEND_s *pYuvDataRdyExt = NULL;
    UINT32 Rval;
    ULONG ULAddr;

    dsp_wrapper_osal_typecast(&pYuvDataRdyExt, &InfoULAddr);

    /* P2V */
    Rval = dsp_wrapper_osal_phys2virt((UINT32)pYuvDataRdyExt->Buffer.BaseAddrY, &ULAddr);
    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
    pYuvDataRdyExt->Buffer.BaseAddrY = ULAddr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_wrapper_osal_phys2virt((UINT32)pYuvDataRdyExt->Buffer.BaseAddrUV, &ULAddr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        pYuvDataRdyExt->Buffer.BaseAddrUV = ULAddr;
    }

    return Rval;
}

static uint32_t DspEventLvPyramidRdy(const ULONG InfoULAddr)
{
    AMBA_DSP_PYMD_DATA_RDY_s *pPymdDataRdy = NULL;
    UINT32 Rval;
    ULONG ULAddr;
    UINT16 i;

    dsp_wrapper_osal_typecast(&pPymdDataRdy, &InfoULAddr);

    /* P2V */
    for (i = 0U; i < AMBA_DSP_MAX_HIER_NUM; i++) {
        Rval = dsp_wrapper_osal_phys2virt((UINT32)pPymdDataRdy->YuvBuf[i].BaseAddrY, &ULAddr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        pPymdDataRdy->YuvBuf[i].BaseAddrY = ULAddr;

        if (Rval == DSP_ERR_NONE) {
            Rval = dsp_wrapper_osal_phys2virt((UINT32)pPymdDataRdy->YuvBuf[i].BaseAddrUV, &ULAddr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            pPymdDataRdy->YuvBuf[i].BaseAddrUV = ULAddr;
        } else {
            break;
        }
    }

    return Rval;
}

static uint32_t DspEventLvLndtRdy(const ULONG InfoULAddr)
{
    AMBA_DSP_LNDT_DATA_RDY_s *pLndtDataRdy = NULL;
    UINT32 Rval;
    ULONG ULAddr;

    dsp_wrapper_osal_typecast(&pLndtDataRdy, &InfoULAddr);

    /* P2V */
    Rval = dsp_wrapper_osal_phys2virt((UINT32)pLndtDataRdy->YuvBuf.BaseAddrY, &ULAddr);
    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
    pLndtDataRdy->YuvBuf.BaseAddrY = ULAddr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_wrapper_osal_phys2virt((UINT32)pLndtDataRdy->YuvBuf.BaseAddrUV, &ULAddr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        pLndtDataRdy->YuvBuf.BaseAddrUV = ULAddr;
    }

    return Rval;
}

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
static uint32_t DspEventLvMainY12Rdy(const ULONG InfoULAddr)
{
    AMBA_DSP_YUV_DATA_RDY_s *pYuvDataRdy = NULL;
    UINT32 Rval;
    ULONG ULAddr;

    dsp_wrapper_osal_typecast(&pYuvDataRdy, &InfoULAddr);

    /* P2V */
    Rval = dsp_wrapper_osal_phys2virt((UINT32)pYuvDataRdy->Buffer.BaseAddrY, &ULAddr);
    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
    pYuvDataRdy->Buffer.BaseAddrY = ULAddr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_wrapper_osal_phys2virt((UINT32)pYuvDataRdy->Buffer.BaseAddrUV, &ULAddr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        pYuvDataRdy->Buffer.BaseAddrUV = ULAddr;
    }

    return Rval;
}
#endif
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
static uint32_t DspEventLvMainIrRdy(const ULONG InfoULAddr)
{
    AMBA_DSP_YUV_DATA_RDY_s *pYuvDataRdy = NULL;
    UINT32 Rval;
    ULONG ULAddr;

    dsp_wrapper_osal_typecast(&pYuvDataRdy, &InfoULAddr);

    /* P2V */
    Rval = dsp_wrapper_osal_phys2virt((UINT32)pYuvDataRdy->Buffer.BaseAddrY, &ULAddr);
    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
    pYuvDataRdy->Buffer.BaseAddrY = ULAddr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_wrapper_osal_phys2virt((UINT32)pYuvDataRdy->Buffer.BaseAddrUV, &ULAddr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        pYuvDataRdy->Buffer.BaseAddrUV = ULAddr;
    }

    return Rval;
}
#endif
#if !defined (CONFIG_SOC_CV2FS) && !defined (CONFIG_SOC_CV22FS) && !defined (CONFIG_SOC_CV5) && !defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
static uint32_t DspEventLvIntMainRdy(const ULONG InfoULAddr)
{
    AMBA_DSP_YUV_DATA_RDY_s *pYuvDataRdy = NULL;
    UINT32 Rval;
    ULONG ULAddr;

    dsp_wrapper_osal_typecast(&pYuvDataRdy, &InfoULAddr);

    /* P2V */
    Rval = dsp_wrapper_osal_phys2virt((UINT32)pYuvDataRdy->Buffer.BaseAddrY, &ULAddr);
    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
    pYuvDataRdy->Buffer.BaseAddrY = ULAddr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_wrapper_osal_phys2virt((UINT32)pYuvDataRdy->Buffer.BaseAddrUV, &ULAddr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        pYuvDataRdy->Buffer.BaseAddrUV = ULAddr;
    }

    return Rval;
}
#endif

static uint32_t DspEventVoutDataRdy(const ULONG InfoULAddr)
{
    AMBA_DSP_VOUT_DATA_INFO_s *pVoutDataRdy = NULL;
    UINT32 Rval;
    ULONG ULAddr;

    dsp_wrapper_osal_typecast(&pVoutDataRdy, &InfoULAddr);

    /* P2V */
    Rval = dsp_wrapper_osal_phys2virt((UINT32)pVoutDataRdy->YuvBuf.BaseAddrY, &ULAddr);
    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
    pVoutDataRdy->YuvBuf.BaseAddrY = ULAddr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_wrapper_osal_phys2virt((UINT32)pVoutDataRdy->YuvBuf.BaseAddrUV, &ULAddr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        pVoutDataRdy->YuvBuf.BaseAddrUV = ULAddr;
    }

    return Rval;
}

static uint32_t DspEventVdoJpgDataRdy(const ULONG InfoULAddr)
{
    AMBA_DSP_ENC_PIC_RDY_s *pEncPicRdy = NULL;
    UINT32 Rval = DSP_ERR_NONE;
    ULONG ULAddr;

    dsp_wrapper_osal_typecast(&pEncPicRdy, &InfoULAddr);

    /* P2V */
    if (pEncPicRdy->PicSize != AMBA_DSP_ENC_END_MARK) {
        Rval = dsp_wrapper_osal_phys2virt((UINT32)pEncPicRdy->StartAddr, &ULAddr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        pEncPicRdy->StartAddr = ULAddr;
    }
    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_wrapper_osal_phys2virt((UINT32)pEncPicRdy->InputYAddr, &ULAddr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        pEncPicRdy->InputYAddr = ULAddr;
    }
    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_wrapper_osal_phys2virt((UINT32)pEncPicRdy->InputUVAddr, &ULAddr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        pEncPicRdy->InputUVAddr = ULAddr;
    }

    return Rval;
}

static uint32_t DspEventStlRawRdy(const ULONG InfoULAddr)
{
    AMBA_DSP_STL_RAW_DATA_INFO_s *pStlRawDataRdy = NULL;
    UINT32 Rval;
    ULONG ULAddr;

    dsp_wrapper_osal_typecast(&pStlRawDataRdy, &InfoULAddr);

    /* P2V */
    Rval = dsp_wrapper_osal_phys2virt((UINT32)pStlRawDataRdy->Buf.BaseAddr, &ULAddr);
    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
    pStlRawDataRdy->Buf.BaseAddr = ULAddr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_wrapper_osal_phys2virt((UINT32)pStlRawDataRdy->AuxBuf.BaseAddr, &ULAddr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        pStlRawDataRdy->AuxBuf.BaseAddr = ULAddr;
    }

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_wrapper_osal_phys2virt((UINT32)pStlRawDataRdy->Aux2Buf.BaseAddr, &ULAddr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        pStlRawDataRdy->Aux2Buf.BaseAddr = ULAddr;
    }

    return Rval;
}

static uint32_t DspEventStlYuvDataRdy(const ULONG InfoULAddr)
{
    AMBA_DSP_YUV_IMG_BUF_s *pStlYuvDataRdy;
    UINT32 Rval;
    ULONG ULAddr;

    dsp_wrapper_osal_typecast(&pStlYuvDataRdy, &InfoULAddr);

    /* P2V */
    Rval = dsp_wrapper_osal_phys2virt((UINT32)pStlYuvDataRdy->BaseAddrY, &ULAddr);
    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
    pStlYuvDataRdy->BaseAddrY = ULAddr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_wrapper_osal_phys2virt((UINT32)pStlYuvDataRdy->BaseAddrUV, &ULAddr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        pStlYuvDataRdy->BaseAddrUV = ULAddr;
    }

    return Rval;
}

static uint32_t DspEventVideoDecStatus(const ULONG InfoULAddr)
{
    AMBA_DSP_EVENT_VIDEO_DEC_STATUS_UPDATE_s *pVidDecStatus = NULL;
    UINT32 Rval;
    ULONG ULAddr;

    dsp_wrapper_osal_typecast(&pVidDecStatus, &InfoULAddr);

    /* P2V */
    Rval = dsp_wrapper_osal_phys2virt((UINT32)pVidDecStatus->BitsNextReadAddr, &ULAddr);
    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
    pVidDecStatus->BitsNextReadAddr = ULAddr;

    return Rval;
}

static uint32_t DspEventVideoDecPicInfo(const ULONG InfoULAddr)
{
    AMBA_DSP_EVENT_VIDDEC_PIC_EX_s *pVidDecPicRdy;
    UINT32 Rval;
    ULONG ULAddr;

    dsp_wrapper_osal_typecast(&pVidDecPicRdy, &InfoULAddr);

    /* P2V */
    Rval = dsp_wrapper_osal_phys2virt((UINT32)pVidDecPicRdy->YAddr, &ULAddr);
    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
    pVidDecPicRdy->YAddr = ULAddr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_wrapper_osal_phys2virt((UINT32)pVidDecPicRdy->UVAddr, &ULAddr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        pVidDecPicRdy->UVAddr = ULAddr;
    }

    return Rval;
}

static uint32_t DspEventStlDecStatus(const ULONG InfoULAddr)
{
    AMBA_DSP_STLDEC_STATUS_s *pStlDecStatus = NULL;
    UINT32 Rval;
    ULONG ULAddr;

    dsp_wrapper_osal_typecast(&pStlDecStatus, &InfoULAddr);

    /* P2V */
    Rval = dsp_wrapper_osal_phys2virt((UINT32)pStlDecStatus->YAddr, &ULAddr);
    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
    pStlDecStatus->YAddr = ULAddr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_wrapper_osal_phys2virt((UINT32)pStlDecStatus->UVAddr, &ULAddr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        pStlDecStatus->UVAddr = ULAddr;
    }

    return Rval;
}

static uint32_t DspEventVdoStlDecYuvDispReport(const ULONG InfoULAddr)
{
    AMBA_DSP_VIDEO_DEC_YUV_DISP_REPORT_s *pPostpDisp = NULL;
    UINT32 Rval;
    ULONG ULAddr;

    dsp_wrapper_osal_typecast(&pPostpDisp, &InfoULAddr);

    /* P2V */
    Rval = dsp_wrapper_osal_phys2virt((UINT32)pPostpDisp->YAddr, &ULAddr);
    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
    pPostpDisp->YAddr = ULAddr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_wrapper_osal_phys2virt((UINT32)pPostpDisp->UVAddr, &ULAddr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        pPostpDisp->UVAddr = ULAddr;
    }

    return Rval;
}

static uint32_t DspEventVideoMvDataRdy(const ULONG InfoULAddr)
{
    AMBA_DSP_ENC_MV_RDY_s *pMvRdy = NULL;
    UINT32 Rval;
    ULONG ULAddr;

    dsp_wrapper_osal_typecast(&pMvRdy, &InfoULAddr);

    /* P2V */
    Rval = dsp_wrapper_osal_phys2virt((UINT32)pMvRdy->MvBufAddr, &ULAddr);
    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
    pMvRdy->MvBufAddr = ULAddr;

    return Rval;
}

static uint32_t DspEventLvSidebandRdy(const ULONG InfoULAddr)
{
    AMBA_DSP_SIDEBAND_DATA_RDY_s *pSideBandRdy = NULL;
    UINT32 Rval;
    ULONG ULAddr;
    UINT16 i;

    dsp_wrapper_osal_typecast(&pSideBandRdy, &InfoULAddr);

    /* P2V */
    for (i = 0U; i < AMBA_DSP_MAX_TOKEN_ARRAY; i++) {
        Rval = dsp_wrapper_osal_phys2virt((UINT32)pSideBandRdy->Info[i].PrivateInfoAddr, &ULAddr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        pSideBandRdy->Info[i].PrivateInfoAddr = ULAddr;
        if (Rval != DSP_ERR_NONE) {
            break;
        }
    }

    return Rval;
}

static uint32_t DspEventStlRawPostprocRdy(const ULONG InfoULAddr)
{
    AMBA_DSP_RAW_DATA_RDY_s *pRawDataRdy = NULL;
    UINT32 Rval;
    ULONG ULAddr;
    UINT16 i;

    dsp_wrapper_osal_typecast(&pRawDataRdy, &InfoULAddr);

    /* P2V */
    Rval = dsp_wrapper_osal_phys2virt((UINT32)pRawDataRdy->RawBuffer.BaseAddr, &ULAddr);
    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
    pRawDataRdy->RawBuffer.BaseAddr = ULAddr;

    if (Rval == DSP_ERR_NONE) {
        Rval = dsp_wrapper_osal_phys2virt((UINT32)pRawDataRdy->AuxBuffer.BaseAddr, &ULAddr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        pRawDataRdy->AuxBuffer.BaseAddr = ULAddr;
    }

    return Rval;
}

static uint32_t DspEventDntRdy(const ULONG InfoULAddr)
{
(void)InfoULAddr;
    return DSP_ERR_NONE;
}

typedef uint32_t (*DSP_EVENT_PROC_f)(const ULONG InfoULAddr);

uint32_t DspWrapGiveEventPreproc(uint16_t EventId, const void *pEventInfo);
uint32_t DspWrapGiveEventPreproc(uint16_t EventId, const void *pEventInfo)
{
    UINT32 Rval = DSP_ERR_0000;
    dsp_stat_buf_t *pEvtBuf = NULL;
    ULONG ULAddr, InfoULAddr;

    static DSP_EVENT_PROC_f DspEventPreproc[AMBA_DSP_EVENT_NUM] = {
            DspEventLvRawRdy,               /* AMBA_DSP_EVENT_LV_RAW_RDY */
            DspEventDntRdy,                 /* AMBA_DSP_EVENT_LV_CFA_AAA_RDY */
            DspEventDntRdy,                 /* AMBA_DSP_EVENT_LV_PG_AAA_RDY */
            DspEventDntRdy,                 /* AMBA_DSP_EVENT_LV_HIST_AAA_RDY */
            DspEventLvYuvDataRdy,           /* AMBA_DSP_EVENT_LV_YUV_DATA_RDY */
            DspEventLvPyramidRdy,           /* AMBA_DSP_EVENT_LV_PYRAMID_RDY */
            DspEventVoutDataRdy,            /* AMBA_DSP_EVENT_VOUT_DATA_RDY */
            DspEventVdoJpgDataRdy,          /* AMBA_DSP_EVENT_VIDEO_DATA_RDY */
            DspEventDntRdy,                 /* AMBA_DSP_EVENT_VIDEO_ENC_START */
            DspEventDntRdy,                 /* AMBA_DSP_EVENT_VIDEO_ENC_STOP */
            DspEventStlRawRdy,              /* AMBA_DSP_EVENT_STL_RAW_RDY */
            DspEventDntRdy,                 /* AMBA_DSP_EVENT_STL_CFA_AAA_RDY */
            DspEventDntRdy,                 /* AMBA_DSP_EVENT_STL_PG_AAA_RDY */
            DspEventStlRawPostprocRdy,      /* AMBA_DSP_EVENT_STL_RAW_POSTPROC_RDY */
            DspEventStlYuvDataRdy,          /* AMBA_DSP_EVENT_STL_YUV_DATA_RDY */
            DspEventVdoJpgDataRdy,          /* AMBA_DSP_EVENT_JPEG_DATA_RDY */
            DspEventDntRdy,                 /* AMBA_DSP_EVENT_VIDEO_PATH_STATUS */
            DspEventVideoDecStatus,         /* AMBA_DSP_EVENT_VIDEO_DEC_STATUS */
            DspEventVdoStlDecYuvDispReport, /* AMBA_DSP_EVENT_VIDEO_DEC_YUV_DISP_REPORT */
            DspEventVideoDecPicInfo,        /* AMBA_DSP_EVENT_VIDEO_DEC_PIC_INFO */
            DspEventStlDecStatus,           /* AMBA_DSP_EVENT_STILL_DEC_STATUS */
            DspEventVdoStlDecYuvDispReport, /* AMBA_DSP_EVENT_STILL_DEC_YUV_DISP_REPORT */
            DspEventDntRdy,                 /* AMBA_DSP_EVENT_VIN_POST_CONFIG */
            DspEventDntRdy,                 /* AMBA_DSP_EVENT_ERROR */
            DspEventDntRdy,                 /* AMBA_DSP_EVENT_LV_SYNC_JOB_RDY */
            DspEventVideoMvDataRdy,         /* AMBA_DSP_EVENT_VIDEO_MV_DATA_RDY */
            DspEventLvSidebandRdy,          /* AMBA_DSP_EVENT_LV_SIDEBAND_RDY */
            DspEventLvLndtRdy,              /* AMBA_DSP_EVENT_LV_LNDT_RDY */
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
            DspEventLvMainY12Rdy,           /* AMBA_DSP_EVENT_LV_MAIN_Y12_RDY */
            DspEventDntRdy,                 /* AMBA_DSP_EVENT_TESTFRAME_RDY */
#elif defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
            DspEventLvMainY12Rdy,           /* AMBA_DSP_EVENT_LV_MAIN_Y12_RDY */
            DspEventLvMainIrRdy,            /* AMBA_DSP_EVENT_LV_MAIN_IR_RDY */
#else
            DspEventLvIntMainRdy,           /* AMBA_DSP_EVENT_LV_INT_MAIN_RDY */
#endif
            DspEventDntRdy                  /* AMBA_DSP_EVENT_CLK_UPDATE_READY */
    };

    if (pEventInfo != NULL) {
        Rval = DSP_ERR_NONE;

        dsp_wrapper_osal_typecast(&pEvtBuf, &pEventInfo);
        /* P2V */
        if (pEvtBuf->Addr > 0U) {
            Rval = dsp_wrapper_osal_phys2virt((UINT32)pEvtBuf->Addr, &InfoULAddr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            pEvtBuf->Addr = InfoULAddr;
            dsp_wrapper_osal_typecast(&pEventInfo, &pEvtBuf);
        } else {
            return DSP_ERR_0000;
        }

        if (EventId < AMBA_DSP_EVENT_NUM) {
            Rval = DspEventPreproc[EventId](InfoULAddr);
        } else {
            Rval = DSP_ERR_0001;
        }
    } else {
        Rval = DSP_ERR_0000;
    }

    return Rval;
}

static inline UINT32 DSP_RelEventInfoPool(UINT16 EvtBufIdx)
{
    UINT32 Rval = 0;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();

    /* ioctl */
    if (dsp_fd > 0) {
        retcode = ioctl(dsp_fd, AMBADSP_EVENT_RELEASE_INFO_POOL, &EvtBufIdx);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

uint32_t DspWrapGiveEvent(uint16_t EventID, const void *pEventInfo)
{
    uint32_t Rval;
    DSP_WRAP_EVENT_HANDLER_CTRL_s   *pEventHdlrCtrl;
    const AMBA_DSP_EVENT_HANDLER_f  *pEventHdlrFunc;
    uint32_t FunctionNum;
    uint32_t i;
    uint32_t PreprocExeced = 0U;
    void *pInfo = NULL;
    dsp_stat_buf_t *pEvtBuf = NULL;
    UINT16 ReleaseBufIdx = 0U;

    Rval = DspWrapEventInit();

    if (Rval == DSP_ERR_NONE) {
        if (DspWrapEventTakeMtx(EventID, &pEventHdlrCtrl) != OK) {
            Rval = DSP_ERR_0003;
        }
    }
    if (pEventInfo == NULL) {
        Rval = DSP_ERR_0000;
    }
    if (Rval == DSP_ERR_NONE) {
        dsp_wrapper_osal_typecast(&pEvtBuf, &pEventInfo);
        ReleaseBufIdx = pEvtBuf->BufIdx;

        pEventHdlrFunc  = pEventHdlrCtrl->Main.pEventHandlers;
        FunctionNum     = pEventHdlrCtrl->Main.MaxNumHandlers;
        for (i = 0U; i < FunctionNum; i++) {
            if (pEventHdlrFunc[i] != NULL) {
                if (PreprocExeced == 0U) {
                    Rval = DspWrapGiveEventPreproc(EventID, pEventInfo);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    PreprocExeced = 1U;
                }

                /* The virtual address of event info */
                if (pEvtBuf->Addr > 0U) {
                    dsp_wrapper_osal_typecast(&pInfo, &pEvtBuf->Addr);
                }
                (void)(pEventHdlrFunc[i])(pInfo);
            }
        }
        Rval = DspWrapEventGiveMtx(pEventHdlrCtrl);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

        /* Release EventInfo pool */
        Rval = DSP_RelEventInfoPool(ReleaseBufIdx);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
    }

    return Rval;
}

/***************************************
 *            AmbaDSP                  *
 ***************************************/
UINT32 AmbaDSP_MainGetDspVerInfo(AMBA_DSP_VERSION_INFO_s *Info)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();

    /* ioctl */
    if (Info == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            retcode = ioctl(dsp_fd, AMBADSP_GET_VERSION, Info);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_MainGetDspBinAddr(AMBA_DSP_BIN_ADDR_s *Info);
UINT32 AmbaDSP_MainGetDspBinAddr(AMBA_DSP_BIN_ADDR_s *Info)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    void *virt_addr;
    uint64_t phys_addr;
    uint32_t len;
    uint32_t flag;
    ULONG ULAddr;
    AMBA_DSP_BUF_INFO_s BufInfo = {0};
    static AMBA_DSP_BIN_ADDR_s DspBinVirtAddr = {0};

    /* ioctl */
    if (Info == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            retcode = ioctl(dsp_fd, AMBADSP_GET_DSP_BIN_ADDR, Info);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            if (Rval == DSP_ERR_NONE) {
                (void)AmbaDSP_MainGetBufInfo(DSP_BUF_TYPE_BIN, &BufInfo);

                /* Phys2Virt */
                if (DspBinVirtAddr.CoreStartAddr == 0U) {
                    len = (uint32_t)(Info->CoreEndAddr - Info->CoreStartAddr);
                    phys_addr = Info->CoreStartAddr;
                    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
                    virt_addr = mmap_device_memory(NULL, len, (int32_t)flag, 0, phys_addr);
                    if (virt_addr == MAP_FAILED) {
                        dsp_wrapper_osal_printI5("[ERR] mmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
                    } else {
                        dsp_wrapper_osal_typecast(&ULAddr, &virt_addr);
                        DspBinVirtAddr.CoreStartAddr = ULAddr;
                        DspBinVirtAddr.CoreEndAddr = ULAddr + len;
                        dsp_wrapper_osal_printU5("=====================DspBin======================", 0U, 0U, 0U, 0U, 0U);
                        dsp_wrapper_osal_printU5("Code: PHYS[0x%X~0x%X] VIRT[0x%X~0x%X]", (UINT32)Info->CoreStartAddr, (UINT32)Info->CoreEndAddr,
                                                                                          (UINT32)DspBinVirtAddr.CoreStartAddr, (UINT32)DspBinVirtAddr.CoreEndAddr, 0U);
                        Rval = dsp_wrapper_osal_add_buf_pool(DSP_WRAP_POOL_BIN_ORC, DspBinVirtAddr.CoreStartAddr, Info->CoreStartAddr, len, 0U);
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    }
                }
                Info->CoreStartAddr = DspBinVirtAddr.CoreStartAddr;
                Info->CoreEndAddr = DspBinVirtAddr.CoreEndAddr;

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
                if (DspBinVirtAddr.Core1StartAddr == 0U) {
                    len = (uint32_t)(Info->Core1EndAddr - Info->Core1StartAddr);
                    phys_addr = Info->Core1StartAddr;
                    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
                    virt_addr = mmap_device_memory(NULL, len, (int32_t)flag, 0, phys_addr);
                    if (virt_addr == MAP_FAILED) {
                        dsp_wrapper_osal_printI5("[ERR] mmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
                    } else {
                        dsp_wrapper_osal_typecast(&ULAddr, &virt_addr);
                        DspBinVirtAddr.Core1StartAddr = ULAddr;
                        DspBinVirtAddr.Core1EndAddr = ULAddr + len;
                        dsp_wrapper_osal_printU5("=====================DspBin======================", 0U, 0U, 0U, 0U, 0U);
                        dsp_wrapper_osal_printU5("Code1:PHYS[0x%X~0x%X] VIRT[0x%X~0x%X]", (UINT32)Info->Core1StartAddr, (UINT32)Info->Core1EndAddr,
                                                                                          (UINT32)DspBinVirtAddr.Core1StartAddr, (UINT32)DspBinVirtAddr.Core1EndAddr, 0U);
                        Rval = dsp_wrapper_osal_add_buf_pool(DSP_WRAP_POOL_BIN_ORC, DspBinVirtAddr.Core1StartAddr, (UINT32)Info->Core1StartAddr, len, 0U);
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    }
                }
                Info->Core1StartAddr = DspBinVirtAddr.Core1StartAddr;
                Info->Core1EndAddr = DspBinVirtAddr.Core1EndAddr;

                if (DspBinVirtAddr.Core2StartAddr == 0U) {
                    len = (uint32_t)(Info->Core2EndAddr - Info->Core2StartAddr);
                    phys_addr = Info->Core2StartAddr;
                    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
                    virt_addr = mmap_device_memory(NULL, len, (int32_t)flag, 0, phys_addr);
                    if (virt_addr == MAP_FAILED) {
                        dsp_wrapper_osal_printI5("[ERR] mmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
                    } else {
                        dsp_wrapper_osal_typecast(&ULAddr, &virt_addr);
                        DspBinVirtAddr.Core2StartAddr = ULAddr;
                        DspBinVirtAddr.Core2EndAddr = ULAddr + len;
                        dsp_wrapper_osal_printU5("=====================DspBin======================", 0U, 0U, 0U, 0U, 0U);
                        dsp_wrapper_osal_printU5("Code2:PHYS[0x%X~0x%X] VIRT[0x%X~0x%X]", (UINT32)Info->Core2StartAddr, (UINT32)Info->Core2EndAddr,
                                                                                          (UINT32)DspBinVirtAddr.Core2StartAddr, (UINT32)DspBinVirtAddr.Core2EndAddr, 0U);
                        Rval = dsp_wrapper_osal_add_buf_pool(DSP_WRAP_POOL_BIN_ORC, DspBinVirtAddr.Core2StartAddr, (UINT32)Info->Core2StartAddr, len, 0U);
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    }
                }
                Info->Core2StartAddr = DspBinVirtAddr.Core2StartAddr;
                Info->Core2EndAddr = DspBinVirtAddr.Core2EndAddr;

                if (DspBinVirtAddr.Core3StartAddr == 0U) {
                    len = (uint32_t)(Info->Core3EndAddr - Info->Core3StartAddr);
                    phys_addr = Info->Core3StartAddr;
                    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
                    virt_addr = mmap_device_memory(NULL, len, (int32_t)flag, 0, phys_addr);
                    if (virt_addr == MAP_FAILED) {
                        dsp_wrapper_osal_printI5("[ERR] mmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
                    } else {
                        dsp_wrapper_osal_typecast(&ULAddr, &virt_addr);
                        DspBinVirtAddr.Core3StartAddr = ULAddr;
                        DspBinVirtAddr.Core3EndAddr = ULAddr + len;
                        dsp_wrapper_osal_printU5("=====================DspBin======================", 0U, 0U, 0U, 0U, 0U);
                        dsp_wrapper_osal_printU5("Code3:PHYS[0x%X~0x%X] VIRT[0x%X~0x%X]", (UINT32)Info->Core3StartAddr, (UINT32)Info->Core3EndAddr,
                                                                                          (UINT32)DspBinVirtAddr.Core3StartAddr, (UINT32)DspBinVirtAddr.Core3EndAddr, 0U);
                        Rval = dsp_wrapper_osal_add_buf_pool(DSP_WRAP_POOL_BIN_ORC, DspBinVirtAddr.Core3StartAddr, (UINT32)Info->Core3StartAddr, len, 0U);
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    }
                }
                Info->Core3StartAddr = DspBinVirtAddr.Core3StartAddr;
                Info->Core3EndAddr = DspBinVirtAddr.Core3EndAddr;

#endif

                if (DspBinVirtAddr.MeStartAddr == 0U) {
                    len = (uint32_t)(Info->MeEndAddr - Info->MeStartAddr);
                    phys_addr = Info->MeStartAddr;
                    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
                    virt_addr = mmap_device_memory(NULL, len, (int32_t)flag, 0, phys_addr);
                    if (virt_addr == MAP_FAILED) {
                        dsp_wrapper_osal_printI5("[ERR] mmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
                    } else {
                        dsp_wrapper_osal_typecast(&ULAddr, &virt_addr);
                        DspBinVirtAddr.MeStartAddr = ULAddr;
                        DspBinVirtAddr.MeEndAddr = ULAddr + len;
                        dsp_wrapper_osal_printU5("Me:   PHYS[0x%X~0x%X] VIRT[0x%X~0x%X]", (UINT32)Info->MeStartAddr, (UINT32)Info->MeEndAddr,
                                                                                          (UINT32)DspBinVirtAddr.MeStartAddr, (UINT32)DspBinVirtAddr.MeEndAddr, 0U);
                        Rval = dsp_wrapper_osal_add_buf_pool(DSP_WRAP_POOL_BIN_ME, DspBinVirtAddr.MeStartAddr, Info->MeStartAddr, len, 0U);
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    }
                }
                Info->MeStartAddr = DspBinVirtAddr.MeStartAddr;
                Info->MeEndAddr = DspBinVirtAddr.MeEndAddr;

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
                if (DspBinVirtAddr.Me1StartAddr == 0U) {
                    len = (uint32_t)(Info->Me1EndAddr - Info->Me1StartAddr);
                    phys_addr = Info->Me1StartAddr;
                    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
                    virt_addr = mmap_device_memory(NULL, len, (int32_t)flag, 0, phys_addr);
                    if (virt_addr == MAP_FAILED) {
                        dsp_wrapper_osal_printI5("[ERR] mmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
                    } else {
                        dsp_wrapper_osal_typecast(&ULAddr, &virt_addr);
                        DspBinVirtAddr.Me1StartAddr = ULAddr;
                        DspBinVirtAddr.Me1EndAddr = ULAddr + len;
                        dsp_wrapper_osal_printU5("Me1:  PHYS[0x%X~0x%X] VIRT[0x%X~0x%X]", (UINT32)Info->Me1StartAddr, (UINT32)Info->Me1EndAddr,
                                                                                          (UINT32)DspBinVirtAddr.Me1StartAddr, (UINT32)DspBinVirtAddr.Me1EndAddr, 0U);
                        Rval = dsp_wrapper_osal_add_buf_pool(DSP_WRAP_POOL_BIN_ME, DspBinVirtAddr.Me1StartAddr, (UINT32)Info->Me1StartAddr, len, 0U);
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    }
                }
                Info->Me1StartAddr = DspBinVirtAddr.Me1StartAddr;
                Info->Me1EndAddr = DspBinVirtAddr.Me1EndAddr;
#endif

                if (DspBinVirtAddr.MdxfStartAddr == 0U) {
                    len = (uint32_t)(Info->MdxfEndAddr - Info->MdxfStartAddr);
                    phys_addr = Info->MdxfStartAddr;
                    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
                    virt_addr = mmap_device_memory(NULL, len, (int32_t)flag, 0, phys_addr);
                    if (virt_addr == MAP_FAILED) {
                        dsp_wrapper_osal_printI5("[ERR] mmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
                    } else {
                        dsp_wrapper_osal_typecast(&ULAddr, &virt_addr);
                        DspBinVirtAddr.MdxfStartAddr = ULAddr;
                        DspBinVirtAddr.MdxfEndAddr = ULAddr + len;
                        dsp_wrapper_osal_printU5("Mdxf: PHYS[0x%X~0x%X] VIRT[0x%X~0x%X]", (UINT32)Info->MdxfStartAddr, (UINT32)Info->MdxfEndAddr,
                                                                                          (UINT32)DspBinVirtAddr.MdxfStartAddr, (UINT32)DspBinVirtAddr.MdxfEndAddr, 0U);
                        Rval = dsp_wrapper_osal_add_buf_pool(DSP_WRAP_POOL_BIN_MDXF, DspBinVirtAddr.MdxfStartAddr, Info->MdxfStartAddr, len, 0U);
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    }
                }
                Info->MdxfStartAddr = DspBinVirtAddr.MdxfStartAddr;
                Info->MdxfEndAddr = DspBinVirtAddr.MdxfEndAddr;

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
                if (DspBinVirtAddr.Mdxf1StartAddr == 0U) {
                    len = (uint32_t)(Info->Mdxf1EndAddr - Info->Mdxf1StartAddr);
                    phys_addr = Info->Mdxf1StartAddr;
                    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
                    virt_addr = mmap_device_memory(NULL, len, (int32_t)flag, 0, phys_addr);
                    if (virt_addr == MAP_FAILED) {
                        dsp_wrapper_osal_printI5("[ERR] mmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
                    } else {
                        dsp_wrapper_osal_typecast(&ULAddr, &virt_addr);
                        DspBinVirtAddr.Mdxf1StartAddr = ULAddr;
                        DspBinVirtAddr.Mdxf1EndAddr = ULAddr + len;
                        dsp_wrapper_osal_printU5("Mdxf1:PHYS[0x%X~0x%X] VIRT[0x%X~0x%X]", (UINT32)Info->Mdxf1StartAddr, (UINT32)Info->Mdxf1EndAddr,
                                                                                          (UINT32)DspBinVirtAddr.Mdxf1StartAddr, (UINT32)DspBinVirtAddr.Mdxf1EndAddr, 0U);
                        Rval = dsp_wrapper_osal_add_buf_pool(DSP_WRAP_POOL_BIN_MDXF, DspBinVirtAddr.Mdxf1StartAddr, (UINT32)Info->Mdxf1StartAddr, len, 0U);
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    }
                }
                Info->Mdxf1StartAddr = DspBinVirtAddr.Mdxf1StartAddr;
                Info->Mdxf1EndAddr = DspBinVirtAddr.Mdxf1EndAddr;
#endif

                if (DspBinVirtAddr.DefaultDataStartAddr == 0U) {
                    len = (uint32_t)(Info->DefaultDataEndAddr - Info->DefaultDataStartAddr);
                    phys_addr = Info->DefaultDataStartAddr;
                    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE | (uint32_t)PROT_NOCACHE;
                    virt_addr = mmap_device_memory(NULL, len, (int32_t)flag, 0, phys_addr);
                    if (virt_addr == MAP_FAILED) {
                        dsp_wrapper_osal_printI5("[ERR] mmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
                    } else {
                        dsp_wrapper_osal_typecast(&ULAddr, &virt_addr);
                        DspBinVirtAddr.DefaultDataStartAddr = ULAddr;
                        DspBinVirtAddr.DefaultDataEndAddr = ULAddr + len;
                        dsp_wrapper_osal_printU5("Deft: PHYS[0x%X~0x%X] VIRT[0x%X~0x%X]", (UINT32)Info->DefaultDataStartAddr, (UINT32)Info->DefaultDataEndAddr,
                                                                                          (UINT32)DspBinVirtAddr.DefaultDataStartAddr, (UINT32)DspBinVirtAddr.DefaultDataEndAddr, 0U);
                        dsp_wrapper_osal_printU5("=================================================", 0U, 0U, 0U, 0U, 0U);
                        Rval = dsp_wrapper_osal_add_buf_pool(DSP_WRAP_POOL_BIN_DEFT, DspBinVirtAddr.DefaultDataStartAddr, Info->DefaultDataStartAddr, len, 0U);
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    }
                }
                Info->DefaultDataStartAddr = DspBinVirtAddr.DefaultDataStartAddr;
                Info->DefaultDataEndAddr = DspBinVirtAddr.DefaultDataEndAddr;
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_MainGetDefaultSysCfg(AMBA_DSP_SYS_CONFIG_s *pDspSysConfig)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode =0;
    int32_t dsp_fd = DSP_GET_FD();

    /* ioctl */
    if (pDspSysConfig == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            retcode = ioctl(dsp_fd, AMBADSP_GET_DEFAULT_SYSCFG, pDspSysConfig);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_MainInit(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    AMBA_DSP_SYS_CONFIG_s DspSysCfg = {0};
    ULONG phys_addr = 0U;
    dsp_sys_drv_cfg_t DspSysDrvCfg = {0};
    uint32_t Freq;

    /* ioctl */
    if (pDspSysConfig == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            Rval = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_IDSP, &Freq);
            if (Rval != DSP_ERR_NONE) {
                dsp_wrapper_osal_printU5("[ERR] AmbaDSP_MainInit() : AmbaSYS_GetClkFreq[IDSP] fail 0x%X", Rval, 0U, 0U, 0U, 0U);
            } else {
                DspSysDrvCfg.Type = DSP_SYS_DRV_CFG_TYPE_IDSP_CLK;
                DspSysDrvCfg.SysDrvCfg.Data = Freq;
                retcode = ioctl(dsp_fd, AMBADSP_SYS_DRV_CFG, &DspSysDrvCfg);
                Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            }

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
            if (Rval == DSP_ERR_NONE) {
#ifdef SUPPORT_AMBA_SYS
                Rval = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_IDSPV, &Freq);
#else
                Freq = 696000000U;
                dsp_wrapper_osal_printU5("AmbaDSP_MainInit() : AmbaSYS_GetClkFreq[IDSPV] n/a %d", Freq, 0U, 0U, 0U, 0U);
#endif
                if (Rval != DSP_ERR_NONE) {
                    dsp_wrapper_osal_printU5("[ERR] AmbaDSP_MainInit() : AmbaSYS_GetClkFreq[IDSPV] fail 0x%X", Rval, 0U, 0U, 0U, 0U);
                } else {
                    DspSysDrvCfg.Type = DSP_SYS_DRV_CFG_TYPE_IDSPV_CLK;
                    DspSysDrvCfg.SysDrvCfg.Data = Freq;
                    retcode = ioctl(dsp_fd, AMBADSP_SYS_DRV_CFG, &DspSysDrvCfg);
                    Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
                }
            }
#endif

            if (Rval == DSP_ERR_NONE) {
                Rval = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_CORE, &Freq);
                if (Rval != DSP_ERR_NONE) {
                    dsp_wrapper_osal_printU5("[ERR] AmbaDSP_MainInit() : AmbaSYS_GetClkFreq[Core] fail 0x%X", Rval, 0U, 0U, 0U, 0U);
                } else {
                    DspSysDrvCfg.Type = DSP_SYS_DRV_CFG_TYPE_CORE_CLK;
                    DspSysDrvCfg.SysDrvCfg.Data = Freq;
                    retcode = ioctl(dsp_fd, AMBADSP_SYS_DRV_CFG, &DspSysDrvCfg);
                    Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
                }
            }

            if (Rval == DSP_ERR_NONE) {
                Rval = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_DRAM, &Freq);
                if (Rval != DSP_ERR_NONE) {
                    dsp_wrapper_osal_printU5("[ERR] AmbaDSP_MainInit() : AmbaSYS_GetClkFreq[DDR] fail 0x%X", Rval, 0U, 0U, 0U, 0U);
                } else {
                    DspSysDrvCfg.Type = DSP_SYS_DRV_CFG_TYPE_DDR_CLK;
                    DspSysDrvCfg.SysDrvCfg.Data = Freq;
                    retcode = ioctl(dsp_fd, AMBADSP_SYS_DRV_CFG, &DspSysDrvCfg);
                    Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
                }
            }

            if (Rval == DSP_ERR_NONE) {
                Rval = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_DSP_SYS, &Freq);
                if (Rval != DSP_ERR_NONE) {
                    dsp_wrapper_osal_printU5("[ERR] AmbaDSP_MainInit() : AmbaSYS_GetClkFreq[AUD] fail 0x%X", Rval, 0U, 0U, 0U, 0U);
                } else {
                    DspSysDrvCfg.Type = DSP_SYS_DRV_CFG_TYPE_AUD_CLK;
                    DspSysDrvCfg.SysDrvCfg.Data = Freq;
                    retcode = ioctl(dsp_fd, AMBADSP_SYS_DRV_CFG, &DspSysDrvCfg);
                    Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
                    if (Rval != DSP_ERR_NONE) {
                        return Rval;
                    }
                }
            }

            Rval = dsp_wrapper_osal_memcpy(&DspSysCfg, pDspSysConfig, sizeof(AMBA_DSP_SYS_CONFIG_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            /* virt2phys */
            Rval = dsp_wrapper_osal_virt2phys(pDspSysConfig->WorkAreaAddr, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_add_buf_pool(DSP_WRAP_POOL_WORK, pDspSysConfig->WorkAreaAddr, phys_addr, DspSysCfg.WorkAreaSize, 1U);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspSysCfg.WorkAreaAddr = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(pDspSysConfig->DebugLogDataAreaAddr, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspSysCfg.DebugLogDataAreaAddr = phys_addr;

    //FIXME
            //DspSysCfg.VpMsgCtrlBufAddr
            if (0U == IS_SUCCESSOR_STATE()) {
                retcode = ioctl(dsp_fd, AMBADSP_MAIN_INIT, &DspSysCfg);
                Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            }
            if (Rval == DSP_ERR_NONE) {
                DspOpMode = DspSysCfg.SysState;
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_MainSuspend(void)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();

    /* ioctl */
    if (dsp_fd > 0) {
        retcode = ioctl(dsp_fd, AMBADSP_MAIN_SUSPEND);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

UINT32 AmbaDSP_MainResume(void)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();

    /* ioctl */
    if (dsp_fd > 0) {
        retcode = ioctl(dsp_fd, AMBADSP_MAIN_RESUME);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

UINT32 AmbaDSP_MainSetWorkArea(ULONG WorkAreaAddr, UINT32 WorkSize)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_mem_t DspMem = {0};
    ULONG phys_addr;

    /* ioctl */
    if (dsp_fd > 0) {
        Rval = dsp_wrapper_osal_virt2phys(WorkAreaAddr, &phys_addr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        DspMem.Addr = phys_addr;

        DspMem.Size = WorkSize;
        retcode = ioctl(dsp_fd, AMBADSP_SET_WORK_MEMORY, &DspMem);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

UINT32 AmbaDSP_MainMsgParseEntry(UINT32 EntryArg)
{
    uint32_t Rval = DSP_ERR_NONE, MsgParseRval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
#ifdef DSP_WRAP_EVENT_GET_DATA_NO_THREAD
    dsp_evnt_data_t DspEventData = {0};
    uint32_t DoWhile = 1U;
    AMBA_DSP_BUF_INFO_s BufInfo = {0};
#endif

    /* ioctl */
    if (dsp_fd > 0) {
#ifndef DSP_WRAP_EVENT_GET_DATA_NO_THREAD
        Rval = dsp_wrapper_osal_eventflag_set(&DspWrapperMsgParseEvent, DSP_WRAPPER_MSG_PARSE_START);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
#endif
        retcode = ioctl(dsp_fd, AMBADSP_MAIN_MSG_PARSE, &EntryArg);
        MsgParseRval = dsp_wrap_get_ioctl_rval(retcode, __func__);

#ifndef DSP_WRAP_EVENT_GET_DATA_NO_THREAD
        Rval = dsp_wrapper_osal_eventflag_set(&DspWrapperMsgParseEvent, DSP_WRAPPER_MSG_PARSE_END);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
#endif
    }

#ifdef DSP_WRAP_EVENT_GET_DATA_NO_THREAD
    while(DoWhile == 1U) {
        /* ioctl */
        if (dsp_fd > 0) {
            retcode = ioctl(dsp_fd, AMBADSP_EVENT_GET_DATA, &DspEventData);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
        if ((Rval == DSP_ERR_NONE) && (DspEventData.EvntDataValid == 1U)) {
            /* Get DspStatBuf */
            if (DspBufAddr[DSP_WRAP_POOL_STAT].Addr == 0U) {
                Rval = AmbaDSP_MainGetBufInfo(DSP_BUF_TYPE_STAT, &BufInfo);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            }
            if (DspBufAddr[DSP_WRAP_POOL_EVT].Addr == 0U) {
                Rval = AmbaDSP_MainGetBufInfo(DSP_BUF_TYPE_EVT, &BufInfo);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            }


            Rval = DspWrapGiveEvent(DspEventData.EventId, &DspEventData.EventData);
        }

        /* All event data be consumed */
        if (DspEventData.PoolEmpty == 1U) {
            DoWhile = 0U;
        }
    }
#endif
    return (Rval|MsgParseRval);
}

UINT32 AmbaDSP_MainWaitVinInterrupt(UINT32 Flag, UINT32 *ActualFlag, UINT32 TimeOut)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_wait_sig_t DspWaitSig = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspWaitSig.Flag = Flag;
        DspWaitSig.TimeOut = TimeOut;
        retcode = ioctl(dsp_fd, AMBADSP_MAIN_WAIT_VIN_INT, &DspWaitSig);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        if (ActualFlag != NULL) {
            *ActualFlag = DspWaitSig.ActualFlag;
        }
    }

    return Rval;
}

UINT32 AmbaDSP_MainWaitVoutInterrupt(UINT32 Flag, UINT32 *ActualFlag, UINT32 TimeOut)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_wait_sig_t DspWaitSig = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspWaitSig.Flag = Flag;
        DspWaitSig.TimeOut = TimeOut;
        retcode = ioctl(dsp_fd, AMBADSP_MAIN_WAIT_VOUT_INT, &DspWaitSig);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        if (ActualFlag != NULL) {
            *ActualFlag = DspWaitSig.ActualFlag;
        }
    }

    return Rval;
}

UINT32 AmbaDSP_MainWaitFlag(UINT32 Flag, UINT32 *ActualFlag, UINT32 TimeOut)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_wait_sig_t DspWaitSig = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspWaitSig.Flag = Flag;
        DspWaitSig.TimeOut = TimeOut;
        retcode = ioctl(dsp_fd, AMBADSP_MAIN_WAIT_FLAG, &DspWaitSig);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        if (ActualFlag != NULL) {
            *ActualFlag = DspWaitSig.ActualFlag;
        }
    }

    return Rval;
}

UINT32 AmbaDSP_ResourceLimitConfig(const AMBA_DSP_RESOURCE_s *pResource)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_resc_limit_cfg_t DspResource = {0};
    dsp_ext_buf_tbl_t DspExtRawBuf = {0};
    uint32_t ext_buf_tbl = 0U;
    uint32_t i, j, k;
    ULONG phys_addr;

    /* ioctl */
    if (pResource == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            Rval = dsp_wrapper_osal_memcpy(&DspResource.EncodeResource, &pResource->EncodeResource, sizeof(AMBA_DSP_ENCODE_RESOURCE_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspResource.StillResource, &pResource->StillResource, sizeof(AMBA_DSP_STILL_RESOURCE_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspResource.DecodeResource, &pResource->DecodeResource, sizeof(AMBA_DSP_DECODE_RESOURCE_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspResource.DisplayResource, &pResource->DisplayResource, sizeof(AMBA_DSP_DISPLAY_RESOURCE_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspResource.ExtMemEncResource, &pResource->ExtMemEncResource, sizeof(AMBA_DSP_EXTMEM_ENC_RESOURCE_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResourceEx, &pResource->LiveviewResourceEx, sizeof(AMBA_DSP_LIVEVIEW_RESOURCE_EX_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            DspResource.LiveviewResource.VideoPipe = pResource->LiveviewResource.VideoPipe;
            DspResource.LiveviewResource.LowDelayMode = pResource->LiveviewResource.LowDelayMode;
            DspResource.LiveviewResource.MaxVinBit = pResource->LiveviewResource.MaxVinBit;
            DspResource.LiveviewResource.MaxViewZoneNum = pResource->LiveviewResource.MaxViewZoneNum;
            Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.MaxVinVirtChanBit[0], &pResource->LiveviewResource.MaxVinVirtChanBit[0], sizeof(UINT16) * AMBA_DSP_MAX_VIN_NUM);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            for (i = 0; i < AMBA_DSP_MAX_VIN_NUM; i++) {
                Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.MaxVinVirtChanOutputNum[i][0], &pResource->LiveviewResource.MaxVinVirtChanOutputNum[i][0], sizeof(UINT16) * AMBA_DSP_MAX_VIRT_CHAN_NUM);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.MaxVinVirtChanWidth[i][0], &pResource->LiveviewResource.MaxVinVirtChanWidth[i][0], sizeof(UINT16) * AMBA_DSP_MAX_VIRT_CHAN_NUM);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.MaxVinVirtChanHeight[i][0], &pResource->LiveviewResource.MaxVinVirtChanHeight[i][0], sizeof(UINT16) * AMBA_DSP_MAX_VIRT_CHAN_NUM);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.DefaultRawBuf[i], &pResource->LiveviewResource.DefaultRawBuf[i], sizeof(AMBA_DSP_RAW_BUF_s) * AMBA_DSP_MAX_VIRT_CHAN_NUM);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.DefaultAuxRawBuf[i], &pResource->LiveviewResource.DefaultAuxRawBuf[i], sizeof(AMBA_DSP_RAW_BUF_s) * AMBA_DSP_MAX_VIRT_CHAN_NUM);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            }
            Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.MaxVinBootTimeout[0], &pResource->LiveviewResource.MaxVinBootTimeout[0], sizeof(UINT32) * AMBA_DSP_MAX_VIN_NUM);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.MaxVinTimeout[0], &pResource->LiveviewResource.MaxVinTimeout[0], sizeof(UINT32) * AMBA_DSP_MAX_VIN_NUM);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.MaxRaw2YuvDelay[0], &pResource->LiveviewResource.MaxRaw2YuvDelay[0], sizeof(UINT16) * AMBA_DSP_MAX_VIN_NUM);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.MaxWarpDma[0], &pResource->LiveviewResource.MaxWarpDma[0], sizeof(UINT16) * AMBA_DSP_MAX_VIEWZONE_NUM);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.MaxWarpWaitLineLuma[0], &pResource->LiveviewResource.MaxWarpWaitLineLuma[0], sizeof(UINT16) * AMBA_DSP_MAX_VIEWZONE_NUM);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.MaxWarpWaitLineChroma[0], &pResource->LiveviewResource.MaxWarpWaitLineChroma[0], sizeof(UINT16) * AMBA_DSP_MAX_VIEWZONE_NUM);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.MaxHierWidth[0], &pResource->LiveviewResource.MaxHierWidth[0], sizeof(UINT16) * AMBA_DSP_MAX_VIEWZONE_NUM);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.MaxHierHeight[0], &pResource->LiveviewResource.MaxHierHeight[0], sizeof(UINT16) * AMBA_DSP_MAX_VIEWZONE_NUM);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.MaxLndtWidth[0], &pResource->LiveviewResource.MaxLndtWidth[0], sizeof(UINT16) * AMBA_DSP_MAX_VIEWZONE_NUM);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.MaxLndtHeight[0], &pResource->LiveviewResource.MaxLndtHeight[0], sizeof(UINT16) * AMBA_DSP_MAX_VIEWZONE_NUM);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspResource.LiveviewResource.MaxVirtVinBit = pResource->LiveviewResource.MaxVirtVinBit;
            Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.MaxVirtVinOutputNum[0], &pResource->LiveviewResource.MaxVirtVinOutputNum[0], sizeof(UINT16) * AMBA_DSP_MAX_VIRT_VIN_NUM);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.MaxVirtVinWidth[0], &pResource->LiveviewResource.MaxVirtVinWidth[0], sizeof(UINT16) * AMBA_DSP_MAX_VIRT_VIN_NUM);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.MaxVirtVinHeight[0], &pResource->LiveviewResource.MaxVirtVinHeight[0], sizeof(UINT16) * AMBA_DSP_MAX_VIRT_VIN_NUM);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.DefaultRawIsoConfigAddr, &pResource->LiveviewResource.DefaultRawIsoConfigAddr, sizeof(AMBA_DSP_DEFAULT_RAW_ISOCFG_CTRL_s) * AMBA_DSP_MAX_VIEWZONE_NUM);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.DefaultRawFormat[0], &pResource->LiveviewResource.DefaultRawFormat[0], sizeof(UINT16) * AMBA_DSP_MAX_VIN_NUM);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            for (i = 0; i < AMBA_DSP_MAX_VIN_NUM; i++) {
                for (j = 0; j < AMBA_DSP_MAX_VIRT_CHAN_NUM; j++) {
                    DspResource.LiveviewResource.RawBuf[i][j].AllocType = pResource->LiveviewResource.RawBuf[i][j].AllocType;
                    DspResource.LiveviewResource.RawBuf[i][j].BufNum = pResource->LiveviewResource.RawBuf[i][j].BufNum;
                    DspResource.LiveviewResource.RawBuf[i][j].RawFormat = pResource->LiveviewResource.RawBuf[i][j].RawFormat;
                    Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.RawBuf[i][j].Buf, &pResource->LiveviewResource.RawBuf[i][j].Buf, sizeof(AMBA_DSP_RAW_BUF_s));
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    Rval = dsp_wrapper_osal_memcpy(&DspResource.LiveviewResource.RawBuf[i][j].AuxBuf, &pResource->LiveviewResource.RawBuf[i][j].AuxBuf, sizeof(AMBA_DSP_RAW_BUF_s));
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

                    Rval = dsp_wrapper_osal_virt2phys(pResource->LiveviewResource.RawBuf[i][j].AuxBuf.BaseAddr, &phys_addr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    DspResource.LiveviewResource.RawBuf[i][j].AuxBuf.BaseAddr = phys_addr;

                    Rval = dsp_wrapper_osal_virt2phys(pResource->LiveviewResource.RawBuf[i][j].Buf.BaseAddr, &phys_addr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    DspResource.LiveviewResource.RawBuf[i][j].Buf.BaseAddr = phys_addr;

                    if (pResource->LiveviewResource.RawBuf[i][j].pRawBufTbl != NULL){
                        ext_buf_tbl += 1U;
                        dsp_wrapper_osal_memset(&DspExtRawBuf, 0, sizeof(dsp_ext_buf_tbl_t));
                        DspExtRawBuf.Type = DSP_EXT_BUF_TYPE_RESC_RAW;
                        DspExtRawBuf.Id = (uint16_t)i;
                        DspExtRawBuf.SubId = (uint16_t)j;
                        for (k = 0; k < pResource->LiveviewResource.RawBuf[i][j].BufNum; k++) {
                            Rval = dsp_wrapper_osal_virt2phys(pResource->LiveviewResource.RawBuf[i][j].pRawBufTbl[k], &phys_addr);
                            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                            DspExtRawBuf.BufTbl[k] = phys_addr;
                        }
                    }

                    if (pResource->LiveviewResource.RawBuf[i][j].pAuxBufTbl != NULL){
                        ext_buf_tbl += 1U;
                        for (k = 0; k < pResource->LiveviewResource.RawBuf[i][j].BufNum; k++) {
                            (void)dsp_wrapper_osal_virt2phys(pResource->LiveviewResource.RawBuf[i][j].pAuxBufTbl[k], &phys_addr);
                            DspExtRawBuf.AuxBufTbl[k] = phys_addr;
                        }
                    }

                    if (ext_buf_tbl > 0U) {
                        retcode = ioctl(dsp_fd, AMBADSP_EXT_RAW_BUF_TBL_CFG, &DspExtRawBuf);
                        ext_buf_tbl = 0U;
                    }

                    Rval = dsp_wrapper_osal_virt2phys(pResource->LiveviewResource.DefaultRawBuf[i][j].BaseAddr, &phys_addr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    DspResource.LiveviewResource.DefaultRawBuf[i][j].BaseAddr = phys_addr;

                    Rval = dsp_wrapper_osal_virt2phys(pResource->LiveviewResource.DefaultAuxRawBuf[i][j].BaseAddr, &phys_addr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    DspResource.LiveviewResource.DefaultAuxRawBuf[i][j].BaseAddr = phys_addr;
                }
            }

            for (i = 0; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
                Rval = dsp_wrapper_osal_virt2phys(pResource->LiveviewResource.DefaultRawIsoConfigAddr[i].CfgAddress, &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspResource.LiveviewResource.DefaultRawIsoConfigAddr[i].CfgAddress = phys_addr;

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
                ViewZoneMaxHorWarpComp[i] = pResource->LiveviewResourceEx.MaxHorWarpCompensation[i];
#endif
            }

            if (0U == IS_SUCCESSOR_STATE()) {
                retcode = ioctl(dsp_fd, AMBADSP_RESC_LIMIT_CONFIG, &DspResource);
                Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_CalHierBufferSize(const UINT16 *pMaxMainWidth, const UINT16 *pMaxMainHeight,
                                 const UINT16 *pMaxHierWidth, const UINT16 *pMaxHierHeight,
                                 const UINT16 OctaveMode,
                                 UINT32 *pHierBufSize, UINT16 *pHierBufWidth, UINT16 *pHierBufHeight)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_hier_buf_calc_t DspHierBufCalc = {0};

    /* ioctl */
    if ((pMaxMainWidth == NULL) || (pMaxMainHeight == NULL) || (pMaxHierWidth == NULL) || (pMaxHierHeight == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspHierBufCalc.MaxMainWidth = *pMaxMainWidth;
            DspHierBufCalc.MaxMainHeight = *pMaxMainHeight;
            DspHierBufCalc.MaxHierWidth = *pMaxHierWidth;
            DspHierBufCalc.MaxHierHeight = *pMaxHierHeight;
            DspHierBufCalc.OctaveMode = OctaveMode;
            retcode = ioctl(dsp_fd, AMBADSP_CALC_HIER_BUF, &DspHierBufCalc);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);

            if (pHierBufSize != NULL) {
                *pHierBufSize = DspHierBufCalc.HierBufSize;
            }
            if (pHierBufWidth != NULL) {
                *pHierBufWidth = DspHierBufCalc.HierBufWidth;
            }
            if (pHierBufHeight != NULL) {
                *pHierBufHeight = DspHierBufCalc.HierBufHeight;
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_GetCmprRawBufInfo(UINT16 Width, UINT16 CmprRate, UINT16 *pRawWidth, UINT16 *pRawPitch)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_raw_pitch_calc_t DspRawPitchCalc = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspRawPitchCalc.Width = Width;
        DspRawPitchCalc.CmprRate = CmprRate;
        retcode = ioctl(dsp_fd, AMBADSP_CALC_RAW_PITCH, &DspRawPitchCalc);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);

        if (pRawWidth != NULL) {
            *pRawWidth = DspRawPitchCalc.RawWidth;
        }
        if (pRawPitch != NULL) {
            *pRawPitch = DspRawPitchCalc.RawPitch;
        }
    }

    return Rval;
}

UINT32 AmbaDSP_ParLoadConfig(UINT32 Enable, UINT32 Data)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_partial_load_cfg_t DspParLoadCfg = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspParLoadCfg.Enable = Enable;
        DspParLoadCfg.Data = Data;
        retcode = ioctl(dsp_fd, AMBADSP_PARLOAD_CONFIG, &DspParLoadCfg);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

UINT32 AmbaDSP_ParLoadRegionUnlock(UINT16 RegionIdx, UINT16 SubRegionIdx)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_partial_load_unlock_t DspParLoadUnlock = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspParLoadUnlock.RegionId = RegionIdx;
        DspParLoadUnlock.SubRegionId = SubRegionIdx;
        retcode = ioctl(dsp_fd, AMBADSP_PARLOAD_REGION_UNLOCK, &DspParLoadUnlock);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

UINT32 AmbaDSP_CalcEncMvBufInfo(UINT16 Width, UINT16 Height, UINT32 Option, UINT32 *pBufSize)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_enc_mv_buf_calc_t DspEncMvBufCalc = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspEncMvBufCalc.Width = Width;
        DspEncMvBufCalc.Height = Height;
        DspEncMvBufCalc.Option = Option;
        retcode = ioctl(dsp_fd, AMBADSP_CALC_ENC_MV_BUF, &DspEncMvBufCalc);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);

        if (pBufSize != NULL) {
            *pBufSize = DspEncMvBufCalc.BufSize;
        }
    }

    return Rval;
}

UINT32 DSP_ParseMvBuf(UINT16 Width, UINT16 Height, UINT16 BufType, UINT32 Option, ULONG MvBufAddr);
UINT32 DSP_ParseMvBuf(UINT16 Width, UINT16 Height, UINT16 BufType, UINT32 Option, ULONG MvBufAddr)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_enc_mv_buf_parse_t DspEncMvBufParse = {0};
    ULONG phys_addr;

    /* ioctl */
    if (dsp_fd > 0) {
        DspEncMvBufParse.Width = Width;
        DspEncMvBufParse.Height = Height;
        DspEncMvBufParse.BufType = BufType;
        DspEncMvBufParse.Option = Option;

        Rval = dsp_wrapper_osal_virt2phys(MvBufAddr, &phys_addr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        DspEncMvBufParse.MvBufAddr = phys_addr;
        retcode = ioctl(dsp_fd, AMBADSP_PARSE_ENC_MV_BUF, &DspEncMvBufParse);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

UINT32 AmbaDSP_CalVpMsgBufferSize(const UINT32 *NumMsgs, UINT32 *MsgSize)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_vp_msg_buf_t DspVpMsgBuf = {0};

    /* ioctl */
    if (NumMsgs == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspVpMsgBuf.NumMsgs = *NumMsgs;
            retcode = ioctl(dsp_fd, AMBADSP_CALC_VP_MSG_BUF, &DspVpMsgBuf);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);

            if (MsgSize != NULL) {
                *MsgSize = DspVpMsgBuf.MsgSize;
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_GetProtectBufInfo(AMBA_DSP_PROTECT_s *pBuf);
UINT32 AmbaDSP_GetProtectBufInfo(AMBA_DSP_PROTECT_s *pBuf)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();

    /* ioctl */
    if (pBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            retcode = ioctl(dsp_fd, AMBADSP_GET_PROTECT_BUF, pBuf);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VirtToPhys(ULONG VirtAddr, UINT32 *pPhysAddr)
{
    UINT32 Rval = DSP_ERR_NONE;
    ULONG PhysAddr;

    Rval = dsp_wrapper_osal_virt2phys(VirtAddr, &PhysAddr);
    if (Rval != DSP_ERR_NONE) {
        dsp_wrapper_osal_printU5("[ERR] AmbaDSP_VirtToPhys: Fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0003;
    } else {
        *pPhysAddr = PhysAddr;
        /* TBD, Phys2Client */
    }

    return Rval;
}

static inline UINT32 AmbaDSP_PhysToVirt(UINT32 PhysAddr, ULONG *pVirtAddr)
{
    UINT32 Rval = DSP_ERR_NONE;
    ULONG VirtAddr;

    Rval = dsp_wrapper_osal_phys2virt((ULONG)PhysAddr, &VirtAddr);
    if (Rval != DSP_ERR_NONE) {
        dsp_wrapper_osal_printU5("[ERR] AmbaDSP_PhysToVirt: Fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0003;
    } else {
        *pVirtAddr = VirtAddr;
        /* TBD, Client2Phys */
    }

    return Rval;
}

UINT32 AmbaDSP_VirtToCli(ULONG VirtAddr, UINT32 *pCliAddr)
{
    UINT32 Rval = DSP_ERR_NONE;

    Rval = AmbaDSP_VirtToPhys(VirtAddr, pCliAddr);

    return Rval;
}

UINT32 AmbaDSP_CliToVirt(UINT32 CliAddr, ULONG *pVirtAddr)
{
    UINT32 Rval = DSP_ERR_NONE;

    Rval = AmbaDSP_PhysToVirt(CliAddr, pVirtAddr);

    return Rval;
}

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
UINT32 AmbaDSP_SliceCfgCalc(UINT16 Id,
                            const AMBA_DSP_WINDOW_DIMENSION_s *pIn,
                            const AMBA_DSP_WINDOW_DIMENSION_s *pOut,
                            AMBA_DSP_SLICE_CFG_s *pSliceCfg)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_slice_cfg_t DspSliceCfg = {0};

    /* ioctl */
    if (pSliceCfg == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspSliceCfg.Id = Id;
            (void)dsp_wrapper_osal_memcpy(&DspSliceCfg.Input, pIn, sizeof(AMBA_DSP_WINDOW_DIMENSION_s));
            (void)dsp_wrapper_osal_memcpy(&DspSliceCfg.Output, pOut, sizeof(AMBA_DSP_WINDOW_DIMENSION_s));
            retcode = ioctl(dsp_fd, AMBADSP_SLICE_CFG_CALC, &DspSliceCfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            if (Rval == DSP_ERR_NONE) {
                Rval = dsp_wrapper_osal_memcpy(pSliceCfg, &DspSliceCfg.SliceCfg, sizeof(AMBA_DSP_SLICE_CFG_s));
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            }
        }
    }

    return Rval;
}
#endif

UINT32 AmbaDSP_MainGetBufInfo(UINT32 Type, AMBA_DSP_BUF_INFO_s *pBufInfo)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_buf_info_t DspBufInfo = {0};
    void *pVirtBuf = NULL;
    ULONG VirtAddr = 0U;
    UINT32 WrapBufType;
    uint32_t flag;

    /* ioctl */
    if (dsp_fd > 0) {
#if defined (CONFIG_DEVICE_TREE_SUPPORT)
        if (Type == DSP_BUF_TYPE_DATA) {
            WrapBufType = DSP_WRAP_POOL_DATA;
        } else
#endif
        if (Type == DSP_BUF_TYPE_STAT) {
            WrapBufType = DSP_WRAP_POOL_STAT;
        } else if (Type == DSP_BUF_TYPE_EVT) {
            WrapBufType = DSP_WRAP_POOL_EVT;
        } else {
            //dsp_wrapper_osal_printU5("[ERR] AmbaDSP_MainGetBufInfo() : unknown buf[%d]", Type, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        }

        if (Rval == DSP_ERR_NONE) {
            if (DspBufAddr[WrapBufType].Addr == 0UL) {
                DspBufInfo.Type = Type;
                retcode = ioctl(dsp_fd, AMBADSP_GET_BUF_INFO, &DspBufInfo);
                Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);

                if (Rval == DSP_ERR_NONE) {
                    pBufInfo->Size = DspBufInfo.Buf.Size;
                    pBufInfo->IsCached = DspBufInfo.Buf.IsCached;

                    /* mmap to get VirtAddr */
                    flag = (uint32_t)PROT_READ | (uint32_t)PROT_WRITE;
                    if (DspBufInfo.Buf.IsCached == 0U) {
                        flag |= (uint32_t)PROT_NOCACHE;
                    }
                    pVirtBuf = mmap_device_memory(NULL, DspBufInfo.Buf.Size, (int32_t)flag, 0, DspBufInfo.Buf.Addr);

                    if (pVirtBuf == MAP_FAILED) {
                        dsp_wrapper_osal_printI5("[ERR] mmap_device_memory fail errno = %d", errno, 0, 0, 0, 0);
                    } else {
                        dsp_wrapper_osal_typecast(&VirtAddr, &pVirtBuf);
                        pBufInfo->BaseAddr = VirtAddr;
                        pBufInfo->BasePhysAddr = DspBufInfo.Buf.Addr;
                        DspBufAddr[WrapBufType].Addr = VirtAddr;
                        DspBufAddr[WrapBufType].Size = DspBufInfo.Buf.Size;
                        DspBufAddr[WrapBufType].IsCached = DspBufInfo.Buf.IsCached;
                        Rval = dsp_wrapper_osal_add_buf_pool(WrapBufType,
                                                            VirtAddr,
                                                            DspBufInfo.Buf.Addr,
                                                            DspBufInfo.Buf.Size,
                                                            DspBufInfo.Buf.IsCached);
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    }
                }
            } else {
                pBufInfo->BaseAddr = DspBufAddr[WrapBufType].Addr;
                pBufInfo->Size = DspBufAddr[WrapBufType].Size;
                pBufInfo->IsCached = DspBufAddr[WrapBufType].IsCached;
                pBufInfo->BasePhysAddr = DspBufAddr[WrapBufType].PhysAddr;
            }
        }
    }

    return Rval;
}

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
UINT32 AmbaDSP_TestFrameConfig(UINT32 StageId, const AMBA_DSP_TEST_FRAME_CFG_s *pCfg)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    ULONG phys_addr;
    uint32_t i;
    dsp_test_frame_config_t DspTfCfg = {0};
    ULONG BitsBufAddr;

    /* ioctl */
    if (pCfg == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspTfCfg.StageId = StageId;
            DspTfCfg.TfCfg.NumTestFrame = pCfg->NumTestFrame;

            Rval = dsp_wrapper_osal_memcpy(&DspTfCfg.TfCfg.Reserved[0U], &pCfg->Reserved[0U], sizeof(UINT8)*3U);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            DspTfCfg.TfCfg.Interval = pCfg->Interval;
            Rval = dsp_wrapper_osal_memcpy(&DspTfCfg.TfCfg.TestFrameCfg[0U], &pCfg->TestFrameCfg[0U], (sizeof(ULONG) * DSP_MAX_TEST_FRAME_NUM));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            for (i = 0; i < DSP_MAX_TEST_FRAME_NUM; i++) {
                if (DspTfCfg.TfCfg.TestFrameCfg[i] > 0U) {
                    Rval = dsp_wrapper_osal_virt2phys(DspTfCfg.TfCfg.TestFrameCfg[i], &phys_addr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    DspTfCfg.TfCfg.TestFrameCfg[i] = phys_addr;
                }
            }

            Rval = dsp_wrapper_osal_memcpy(&DspTfCfg.TfCfg.TestFrameInp[0U], &pCfg->TestFrameInp[0U], (sizeof(ULONG) * DSP_MAX_TEST_INP_MAX));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            for (i = 0; i < DSP_MAX_TEST_INP_MAX; i++) {
                if (DspTfCfg.TfCfg.TestFrameInp[i] > 0U) {
                    Rval = dsp_wrapper_osal_virt2phys(DspTfCfg.TfCfg.TestFrameInp[i], &phys_addr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    DspTfCfg.TfCfg.TestFrameInp[i] = phys_addr;
                }
            }

            if (pCfg->pBitsBufAddr != NULL){
                dsp_wrapper_osal_typecast(&BitsBufAddr, &pCfg->pBitsBufAddr);
                Rval = dsp_wrapper_osal_virt2phys(BitsBufAddr, &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspTfCfg.TfCfg.BitsBufAddr = phys_addr;
            } else {
                DspTfCfg.TfCfg.BitsBufAddr = 0U;
            }
            DspTfCfg.TfCfg.BitsBufSize = pCfg->BitsBufSize;

            retcode = ioctl(dsp_fd, AMBADSP_TEST_FRAME_CFG, &DspTfCfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_TestFrameCtrl(UINT32 NumStage, const AMBA_DSP_TEST_FRAME_CTRL_s *pCtrl)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    uint32_t phys_addr, i;
    dsp_test_frame_control_t DspTfCtrl = {0};

    /* ioctl */
    if (pCtrl == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspTfCtrl.NumStage = NumStage;

            Rval = dsp_wrapper_osal_memcpy(&DspTfCtrl.TfCtrl, pCtrl, sizeof(AMBA_DSP_TEST_FRAME_CTRL_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            retcode = ioctl(dsp_fd, AMBADSP_TEST_FRAME_CTRL, &DspTfCtrl);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_MainSafetyChk(const UINT32 ID, const UINT32 SubID, UINT32 *pStatus)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_main_safety_check_t DspSafetyChk = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        if (pStatus == NULL) {
            dsp_wrapper_osal_printU5("[ERR] AmbaDSP_MainSafetyChk() : Null pStatus", 0U, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        } else if ((ID >= DSP_RESET_STATUS_NUM) || (SubID > 1U)) {
            dsp_wrapper_osal_printU5("[ERR] AmbaDSP_MainSafetyChk() : Invalid ID[%d] or SubID[%d]", ID, SubID, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        } else {
            DspSafetyChk.Id = ID;
            DspSafetyChk.SubId = SubID;
            retcode = ioctl(dsp_fd, AMBADSP_MAIN_SAFETY_CHECK, &DspSafetyChk);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);

            if (pStatus != NULL) {
                *pStatus = DspSafetyChk.Status;
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_MainSafetyCfg(const UINT32 ID, const UINT32 Val0, const UINT32 Val1)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_main_safety_config_t DspSafetyCfg = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        if (ID > DSP_SAFETY_ENET) {
            dsp_wrapper_osal_printU5("[ERR] AmbaDSP_MainSafetyCfg() : Invalid ID[%d] Val0[%d] Val1[%d]", ID, Val0, Val1, 0U, 0U);
            Rval = DSP_ERR_0001;
        } else {
            DspSafetyCfg.Id = ID;
            DspSafetyCfg.Val0 = Val0;
            DspSafetyCfg.Val1 = Val1;
            retcode = ioctl(dsp_fd, AMBADSP_MAIN_SAFETY_CONFIG, &DspSafetyCfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }
    return Rval;
}
#endif

UINT32 AmbaDSP_MainSetProtectArea(UINT32 Type, ULONG AreaAddr, UINT32 Size, UINT32 IsCached)
{
    uint32_t Rval = DSP_ERR_NONE;
#if defined (CONFIG_DEVICE_TREE_SUPPORT)
(void)Type;
(void)AreaAddr;
(void)Size;
(void)IsCached;
#else
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_protect_area_t DspProtectArea = {0};
    ULONG phys_addr = 0UL;

    /* ioctl */
    if (dsp_fd > 0) {
        DspProtectArea.Type = Type;
        DspProtectArea.Addr = AreaAddr;
        DspProtectArea.Size = Size;
        DspProtectArea.IsCached = IsCached;

        Rval = dsp_wrapper_osal_virt2phys(DspProtectArea.Addr , &phys_addr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        if (Type == DSP_PROTECT_AREA_DATA) {
            Rval = dsp_wrapper_osal_add_buf_pool(DSP_WRAP_POOL_DATA, AreaAddr, phys_addr, Size, IsCached);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        }
        DspProtectArea.Addr = phys_addr;

        retcode = ioctl(dsp_fd, AMBADSP_SET_PROTECT_AREA, &DspProtectArea);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }
#endif
    return Rval;
}

UINT32 AmbaDSP_MainApiDumpInit(UINT8 Enable, UINT32 Mask);
UINT32 AmbaDSP_MainApiDumpInit(UINT8 Enable, UINT32 Mask)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_debug_dump_t DspDebugDump = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspDebugDump.Type = DSP_DBG_TYPE_API;
        DspDebugDump.Data[0U] = Enable;
        DspDebugDump.Data[1U] = Mask;
        retcode = ioctl(dsp_fd, AMBADSP_DEBUG_DUMP, &DspDebugDump);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

UINT32 AmbaDSP_MainApiCheckInit(UINT8 Disable, UINT32 Mask);
UINT32 AmbaDSP_MainApiCheckInit(UINT8 Disable, UINT32 Mask)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_debug_dump_t DspDebugDump = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspDebugDump.Type = DSP_DBG_TYPE_API_CHK;
        DspDebugDump.Data[0U] = Disable;
        DspDebugDump.Data[1U] = Mask;
        retcode = ioctl(dsp_fd, AMBADSP_DEBUG_DUMP, &DspDebugDump);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

void AmbaDSP_CmdShow(UINT32 CmdCode, UINT8 On);
void AmbaDSP_CmdShow(UINT32 CmdCode, UINT8 On)
{
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_debug_dump_t DspDebugDump = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspDebugDump.Type = DSP_DBG_TYPE_CMD;
        DspDebugDump.Data[0U] = CmdCode;
        DspDebugDump.Data[1U] = On;
        retcode = ioctl(dsp_fd, AMBADSP_DEBUG_DUMP, &DspDebugDump);
    }

    return;
}

void AmbaDSP_CmdShowAll(UINT8 On);
void AmbaDSP_CmdShowAll(UINT8 On)
{
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_debug_dump_t DspDebugDump = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspDebugDump.Type = DSP_DBG_TYPE_CMD_ALL;
        DspDebugDump.Data[0U] = On;
        retcode = ioctl(dsp_fd, AMBADSP_DEBUG_DUMP, &DspDebugDump);
    }

    return;
}

void AmbaDSP_MsgShow(UINT32 MsgCode, UINT8 On);
void AmbaDSP_MsgShow(UINT32 MsgCode, UINT8 On)
{
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_debug_dump_t DspDebugDump = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspDebugDump.Type = DSP_DBG_TYPE_MSG;
        DspDebugDump.Data[0U] = MsgCode;
        DspDebugDump.Data[1U] = On;
        retcode = ioctl(dsp_fd, AMBADSP_DEBUG_DUMP, &DspDebugDump);
    }

    return;
}

void AmbaDSP_MsgShowAll(UINT8 On);
void AmbaDSP_MsgShowAll(UINT8 On)
{
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_debug_dump_t DspDebugDump = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspDebugDump.Type = DSP_DBG_TYPE_MSG_ALL;
        DspDebugDump.Data[0U] = On;
        retcode = ioctl(dsp_fd, AMBADSP_DEBUG_DUMP, &DspDebugDump);
    }

    return;
}

UINT32 AmbaDSP_SetDebugThread(UINT32 ThreadValid, UINT32 ThreadMask, UINT8 CmdType);
UINT32 AmbaDSP_SetDebugThread(UINT32 ThreadValid, UINT32 ThreadMask, UINT8 CmdType)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_debug_dump_t DspDebugDump = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspDebugDump.Type = DSP_DBG_TYPE_LOG_THRD;
        DspDebugDump.Data[0U] = ThreadValid;
        DspDebugDump.Data[1U] = ThreadMask;
        DspDebugDump.Data[2U] = CmdType;
        retcode = ioctl(dsp_fd, AMBADSP_DEBUG_DUMP, &DspDebugDump);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

UINT32 AmbaDSP_SetDebugLevel(UINT32 Module, UINT32 Level, UINT8 CmdType);
UINT32 AmbaDSP_SetDebugLevel(UINT32 Module, UINT32 Level, UINT8 CmdType)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_debug_dump_t DspDebugDump = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspDebugDump.Type = DSP_DBG_TYPE_LOG_LVL;
        DspDebugDump.Data[0U] = Module;
        DspDebugDump.Data[1U] = Level;
        DspDebugDump.Data[2U] = CmdType;
        retcode = ioctl(dsp_fd, AMBADSP_DEBUG_DUMP, &DspDebugDump);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

void AmbaDSP_LogShow(UINT32 Line);
void AmbaDSP_LogShow(UINT32 Line)
{
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_debug_dump_t DspDebugDump = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspDebugDump.Type = DSP_DBG_TYPE_LL_LOG_SHOW;
        DspDebugDump.Data[0U] = Line;
        retcode = ioctl(dsp_fd, AMBADSP_DEBUG_DUMP, &DspDebugDump);
    }

    return;
}

void AmbaDSP_InitDataDump(void);
void AmbaDSP_InitDataDump(void)
{
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_debug_dump_t DspDebugDump = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspDebugDump.Type = DSP_DBG_TYPE_INIT_DATA_SHOW;
        retcode = ioctl(dsp_fd, AMBADSP_DEBUG_DUMP, &DspDebugDump);
    }

    return;
}

UINT32 AmbaDSP_Stop(UINT32 CmdType);
UINT32 AmbaDSP_Stop(UINT32 CmdType)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_debug_dump_t DspDebugDump = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspDebugDump.Type = DSP_DBG_TYPE_STOP_DSP;
        DspDebugDump.Data[0U] = CmdType;
        retcode = ioctl(dsp_fd, AMBADSP_DEBUG_DUMP, &DspDebugDump);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

UINT32 AmbaDSP_SetSystemCfg(UINT8 ParIdx, UINT32 Val);
UINT32 AmbaDSP_SetSystemCfg(UINT8 ParIdx, UINT32 Val)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_debug_dump_t DspDebugDump = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspDebugDump.Type = DSP_DBG_TYPE_SYSCFG;
        DspDebugDump.Data[0U] = ParIdx;
        DspDebugDump.Data[1U] = Val;
        retcode = ioctl(dsp_fd, AMBADSP_DEBUG_DUMP, &DspDebugDump);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

UINT32 AmbaDSP_GetStatus(UINT32 Type, UINT32 Id, UINT32 *pData0, UINT32 *pData1)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_status_t DspStatus = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspStatus.Type = Type;
        DspStatus.Id = Id;
        retcode = ioctl(dsp_fd, AMBADSP_GET_STATUS, &DspStatus);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        if (Rval == DSP_ERR_NONE) {
            *pData0 = DspStatus.Data[0U];
            *pData1 = DspStatus.Data[1U];
        }
    }

    return Rval;
}

UINT32 AmbaDSP_EventHandlerCtrlConfig(UINT16 EventID, UINT16 MaxNumHandlers, AMBA_DSP_EVENT_HANDLER_f *pEventHandlers)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_evnt_hdlr_cfg_t DspEvntHdlrCfg = {0};
(void)pEventHandlers;

    /* ioctl */
    if (dsp_fd > 0) {
        DspEvntHdlrCfg.EventId = EventID;
        DspEvntHdlrCfg.MaxNumHandlers = MaxNumHandlers;
        retcode = ioctl(dsp_fd, AMBADSP_EVENT_HDLR_CONFIG, &DspEvntHdlrCfg);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }
    if (Rval == DSP_ERR_NONE) {
        Rval = DspWrapEventHandlerConfig(EventID, MaxNumHandlers, pEventHandlers);
    }

    return Rval;
}

UINT32 AmbaDSP_EventHandlerCtrlReset(UINT16 EventID)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();

    /* ioctl */
    if (dsp_fd > 0) {
        retcode = ioctl(dsp_fd, AMBADSP_EVENT_HDLR_RESET, &EventID);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }
    if (Rval == DSP_ERR_NONE) {
        Rval = DspWrapEventHandlerReset(EventID);
    }

    return Rval;
}

UINT32 AmbaDSP_EventHandlerRegister(UINT16 EventID, AMBA_DSP_EVENT_HANDLER_f EventHandler)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_evnt_hdlr_op_t DspEvntHdlrOp = {0};
(void)EventHandler;

    /* ioctl */
    if (dsp_fd > 0) {
        DspEvntHdlrOp.EventId = EventID;
        retcode = ioctl(dsp_fd, AMBADSP_EVENT_HDLR_REGISTER, &DspEvntHdlrOp);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }
    if (Rval == DSP_ERR_NONE) {
        Rval = DspWrapRegisterEventHandler(EventID, EventHandler);
    }

    return Rval;
}

UINT32 AmbaDSP_EventHandlerUnRegister(UINT16 EventID, AMBA_DSP_EVENT_HANDLER_f EventHandler)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_evnt_hdlr_op_t DspEvntHdlrOp = {0};
(void)EventHandler;

    /* ioctl */
    if (dsp_fd > 0) {
        DspEvntHdlrOp.EventId = EventID;
        retcode = ioctl(dsp_fd, AMBADSP_EVENT_HDLR_DEREGISTER, &DspEvntHdlrOp);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }
    if (Rval == DSP_ERR_NONE) {
        Rval = DspWrapUnRegisterEventHandler(EventID, EventHandler);
    }

    return Rval;
}

UINT32 AmbaDSP_VoutReset(const UINT8 VoutIdx)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();

    /* ioctl */
    if (dsp_fd > 0) {
        retcode = ioctl(dsp_fd, AMBADSP_VOUT_RESET, &VoutIdx);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

UINT32 AmbaDSP_VoutMixerConfig(const UINT8 VoutIdx, const AMBA_DSP_VOUT_MIXER_CONFIG_s *pConfig)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_vout_mixer_cfg_t DspVoutMixerCfg = {0};

    /* ioctl */
    if (pConfig == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspVoutMixerCfg.VoutIdx = VoutIdx;
            Rval = dsp_wrapper_osal_memcpy(&DspVoutMixerCfg.MixerCfg, pConfig, sizeof(AMBA_DSP_VOUT_MIXER_CONFIG_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            retcode = ioctl(dsp_fd, AMBADSP_VOUT_MIXER_CFG, &DspVoutMixerCfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VoutMixerConfigBackColor(const UINT8 VoutIdx, UINT32 BackColorYUV)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_vout_mixer_bgc_cfg_t DspVoutMixerBgcCfg = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspVoutMixerBgcCfg.VoutIdx = VoutIdx;
        DspVoutMixerBgcCfg.BackColorYUV = BackColorYUV;
        retcode = ioctl(dsp_fd, AMBADSP_VOUT_MIXER_BGC_CFG, &DspVoutMixerBgcCfg);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

UINT32 AmbaDSP_VoutMixerConfigHighlightColor(const UINT8 VoutIdx, UINT8 LumaThreshold, UINT32 HighlightColorYUV)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_vout_mixer_hlc_cfg_t DspVoutMixerHlcCfg = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspVoutMixerHlcCfg.VoutIdx = VoutIdx;
        DspVoutMixerHlcCfg.LumaThreshold = LumaThreshold;
        DspVoutMixerHlcCfg.HighlightColorYUV = HighlightColorYUV;
        retcode = ioctl(dsp_fd, AMBADSP_VOUT_MIXER_HLC_CFG, &DspVoutMixerHlcCfg);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

UINT32 AmbaDSP_VoutMixerConfigCsc(const UINT8 VoutIdx, const UINT8 CscCtrl)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_vout_mixer_csc_cfg_t DspVoutMixerCscCfg = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspVoutMixerCscCfg.VoutIdx = VoutIdx;
        DspVoutMixerCscCfg.CscCtrl = CscCtrl;
        retcode = ioctl(dsp_fd, AMBADSP_VOUT_MIXER_CSC_CFG, &DspVoutMixerCscCfg);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

static inline INT32 Float2Int32(FLOAT WorkFLOAT)
{
    INT32 WorkINT;

    if (WorkFLOAT >= 0.0F) {
        WorkINT = (INT32)(WorkFLOAT);
    } else {
        WorkINT = 0 - (INT32)(-WorkFLOAT);
    }
    return WorkINT;
}

UINT32 AmbaDSP_VoutMixerCscMatrixConfig(const UINT8 VoutIdx, const AMBA_DSP_VOUT_CSC_MATRIX_s *pCscMatrix)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_vout_mixer_csc_matrix_cfg_t DspVoutMixerCscMatrixCfg = {0};

    /* ioctl */
    if (pCscMatrix == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspVoutMixerCscMatrixCfg.VoutIdx = VoutIdx;
            DspVoutMixerCscMatrixCfg.CscMatrix.Coef[0U][0U] = Float2Int32(pCscMatrix->Coef[0U][0U] * 1024.0F);
            DspVoutMixerCscMatrixCfg.CscMatrix.Coef[0U][1U] = Float2Int32(pCscMatrix->Coef[0U][1U] * 1024.0F);
            DspVoutMixerCscMatrixCfg.CscMatrix.Coef[0U][2U] = Float2Int32(pCscMatrix->Coef[0U][2U] * 1024.0F);
            DspVoutMixerCscMatrixCfg.CscMatrix.Coef[1U][0U] = Float2Int32(pCscMatrix->Coef[1U][0U] * 1024.0F);
            DspVoutMixerCscMatrixCfg.CscMatrix.Coef[1U][1U] = Float2Int32(pCscMatrix->Coef[1U][1U] * 1024.0F);
            DspVoutMixerCscMatrixCfg.CscMatrix.Coef[1U][2U] = Float2Int32(pCscMatrix->Coef[1U][2U] * 1024.0F);
            DspVoutMixerCscMatrixCfg.CscMatrix.Coef[2U][0U] = Float2Int32(pCscMatrix->Coef[2U][0U] * 1024.0F);
            DspVoutMixerCscMatrixCfg.CscMatrix.Coef[2U][1U] = Float2Int32(pCscMatrix->Coef[2U][1U] * 1024.0F);
            DspVoutMixerCscMatrixCfg.CscMatrix.Coef[2U][2U] = Float2Int32(pCscMatrix->Coef[2U][2U] * 1024.0F);
            DspVoutMixerCscMatrixCfg.CscMatrix.Offset[0U]   = Float2Int32(pCscMatrix->Offset[0U]);
            DspVoutMixerCscMatrixCfg.CscMatrix.Offset[1U]   = Float2Int32(pCscMatrix->Offset[1U]);
            DspVoutMixerCscMatrixCfg.CscMatrix.Offset[2U]   = Float2Int32(pCscMatrix->Offset[2U]);
            DspVoutMixerCscMatrixCfg.CscMatrix.MinVal[0U]   = pCscMatrix->MinVal[0U];
            DspVoutMixerCscMatrixCfg.CscMatrix.MaxVal[0U]   = pCscMatrix->MaxVal[0U];
            DspVoutMixerCscMatrixCfg.CscMatrix.MinVal[1U]   = pCscMatrix->MinVal[1U];
            DspVoutMixerCscMatrixCfg.CscMatrix.MaxVal[1U]   = pCscMatrix->MaxVal[1U];
            DspVoutMixerCscMatrixCfg.CscMatrix.MinVal[2U]   = pCscMatrix->MinVal[2U];
            DspVoutMixerCscMatrixCfg.CscMatrix.MaxVal[2U]   = pCscMatrix->MaxVal[2U];
            retcode = ioctl(dsp_fd, AMBADSP_VOUT_MIXER_CSC_MATRIX_CFG, &DspVoutMixerCscMatrixCfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VoutMixerCtrl(const UINT8 VoutIdx)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();

    /* ioctl */
    if (dsp_fd > 0) {
        if (0U == IS_SUCCESSOR_STATE()) {
            retcode = ioctl(dsp_fd, AMBADSP_VOUT_MIXER_CTRL, &VoutIdx);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VoutOsdConfigBuf(const UINT8 VoutIdx, const AMBA_DSP_VOUT_OSD_BUF_CONFIG_s *pBufConfig)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_vout_osd_buf_cfg_t DspVoutOsdBufCfg = {0};
    ULONG phys_addr = 0UL;

    /* ioctl */
    if (pBufConfig == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspVoutOsdBufCfg.VoutIdx = VoutIdx;
            Rval = dsp_wrapper_osal_memcpy(&DspVoutOsdBufCfg.BufCfg, pBufConfig, sizeof(AMBA_DSP_VOUT_OSD_BUF_CONFIG_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            /* virt2phys */
            Rval = dsp_wrapper_osal_virt2phys(DspVoutOsdBufCfg.BufCfg.BaseAddr, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspVoutOsdBufCfg.BufCfg.BaseAddr = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspVoutOsdBufCfg.BufCfg.CLUTAddr, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspVoutOsdBufCfg.BufCfg.CLUTAddr = phys_addr;

            retcode = ioctl(dsp_fd, AMBADSP_VOUT_OSD_BUF_CFG, &DspVoutOsdBufCfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VoutOsdCtrl(const UINT8 VoutIdx, UINT8 Enable, UINT8 SyncWithVin, UINT64 *pAttachedRawSeq)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_vout_osd_ctrl_t DspVoutOsdCtrl = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspVoutOsdCtrl.VoutIdx = VoutIdx;
        DspVoutOsdCtrl.Enable = Enable;
        DspVoutOsdCtrl.SyncWithVin = SyncWithVin;
        retcode = ioctl(dsp_fd, AMBADSP_VOUT_OSD_CTRL, &DspVoutOsdCtrl);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);

        if (pAttachedRawSeq != NULL) {
            *pAttachedRawSeq = DspVoutOsdCtrl.AttachedRawSeq;
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VoutDisplayConfig(const UINT8 VoutIdx, const AMBA_DSP_DISPLAY_CONFIG_s* pConfig)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_vout_drv_cfg_t DspVoutDrvCfg = {0};
    dsp_vout_disp_cfg_t DspVoutDispCfg = {0};
    ULONG DrvCfgAddr;
    uint32_t DrvCfgSize, DataSize;
    void *pVoid;

    /* ioctl */
    if (pConfig == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            /* Display driver config */
            if (VoutIdx == VOUT_IDX_A) {
                Rval = AmbaVout_GetDisp0ConfigAddr(&DrvCfgAddr, &DrvCfgSize);
            } else {
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
                if ((VoutIdx == VOUT_IDX_B) &&
                    (VoutDisplayOption[VoutIdx] == 1U)) {
                    Rval = AmbaVout_GetDisp1ConfigAddr(&DrvCfgAddr, &DrvCfgSize);
                } else {
                    Rval = AmbaVout_GetDisp2ConfigAddr(&DrvCfgAddr, &DrvCfgSize);
                }
#else
                Rval = AmbaVout_GetDisp1ConfigAddr(&DrvCfgAddr, &DrvCfgSize);
#endif
            }
            if (Rval != DSP_ERR_NONE) {
                dsp_wrapper_osal_printU5("[ERR] AmbaDSP_VoutDisplayConfig() : Vout_GetDispConfigAddr[%d] fail 0x%X", VoutIdx, Rval, 0U, 0U, 0U);
            } else {
                DataSize = (uint32_t)sizeof(DspVoutDrvCfg.VoutDrvCfg.Data);
                if (DrvCfgSize > DataSize) {
                    dsp_wrapper_osal_printU5("[ERR] AmbaDSP_VoutDisplayConfig() : Vout_GetDispConfigAddr[%d] fail Data size exceed %d %d", VoutIdx, DrvCfgSize, DataSize, 0U, 0U);
                    Rval = DSP_ERR_0001;
                }
            }

            if (Rval == DSP_ERR_NONE) {
                DspVoutDrvCfg.VoutId = VoutIdx;
                DspVoutDrvCfg.Type = DSP_VOUT_DRV_CFG_TYPE_DISP;
                dsp_wrapper_osal_typecast(&pVoid, &DrvCfgAddr);
                Rval = dsp_wrapper_osal_memcpy(DspVoutDrvCfg.VoutDrvCfg.Data, pVoid, DrvCfgSize);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                retcode = ioctl(dsp_fd, AMBADSP_VOUT_DRV_CFG, &DspVoutDrvCfg);
                Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            }

            if (VoutIdx == VOUT_IDX_A) {
                Rval = AmbaVout_GetDisp0DigiCscAddr(&DrvCfgAddr, &DrvCfgSize);
            } else {
#if defined (CONFIG_SOC_CV2FS)
                Rval = AmbaVout_GetDisp1DigiCscAddr(&DrvCfgAddr, &DrvCfgSize);
#elif defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
                if ((VoutIdx == VOUT_IDX_B) &&
                    (VoutDisplayOption[VoutIdx] == 1U)) {
                    Rval = AmbaVout_GetDisp1DigiCscAddr(&DrvCfgAddr, &DrvCfgSize);
                } else {
                    Rval = AmbaVout_GetDisp2HdmiCscAddr(&DrvCfgAddr, &DrvCfgSize);
                }
#else
                Rval = AmbaVout_GetDisp1HdmiCscAddr(&DrvCfgAddr, &DrvCfgSize);
#endif
            }
            if (Rval != DSP_ERR_NONE) {
                dsp_wrapper_osal_printU5("[ERR] AmbaDSP_VoutDisplayConfig() : Vout_GetDispCscAddr[%d] fail 0x%X", VoutIdx, Rval, 0U, 0U, 0U);
            } else {
                DataSize = (uint32_t)sizeof(DspVoutDrvCfg.VoutDrvCfg.Data);
                if (DrvCfgSize > DataSize) {
                    dsp_wrapper_osal_printU5("[ERR] AmbaDSP_VoutDisplayConfig() : Vout_GetDispCscAddr[%d] fail Data size exceed %d %d", VoutIdx, DrvCfgSize, DataSize, 0U, 0U);
                    Rval = DSP_ERR_0001;
                }
            }

            if (Rval == DSP_ERR_NONE) {
                DspVoutDrvCfg.VoutId = VoutIdx;
                DspVoutDrvCfg.Type = DSP_VOUT_DRV_CFG_TYPE_MIXER_CSC;
                dsp_wrapper_osal_typecast(&pVoid, &DrvCfgAddr);
                Rval = dsp_wrapper_osal_memcpy(DspVoutDrvCfg.VoutDrvCfg.Data, pVoid, DrvCfgSize);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                retcode = ioctl(dsp_fd, AMBADSP_VOUT_DRV_CFG, &DspVoutDrvCfg);
                Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            }

            DspVoutDispCfg.VoutIdx = VoutIdx;
            Rval = dsp_wrapper_osal_memcpy(&DspVoutDispCfg.Cfg, pConfig, sizeof(AMBA_DSP_DISPLAY_CONFIG_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            retcode = ioctl(dsp_fd, AMBADSP_VOUT_DISP_CFG, &DspVoutDispCfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VoutDisplayCtrl(const UINT8 VoutIdx)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();

    /* ioctl */
    if (dsp_fd > 0) {
        if (0U == IS_SUCCESSOR_STATE()) {
            retcode = ioctl(dsp_fd, AMBADSP_VOUT_DISP_CTRL, &VoutIdx);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VoutDisplayConfigGamma(const UINT8 VoutIdx, ULONG TableAddr)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_vout_disp_gamma_cfg_t DspVoutDispGammaCfg = {0};
    ULONG phys_addr;

    /* ioctl */
    if (dsp_fd > 0) {
        DspVoutDispGammaCfg.VoutIdx = VoutIdx;

        /* virt2phys */
        Rval = dsp_wrapper_osal_virt2phys(TableAddr, &phys_addr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        DspVoutDispGammaCfg.TableAddr = phys_addr;

        retcode = ioctl(dsp_fd, AMBADSP_VOUT_DISP_GAMMA_CFG, &DspVoutDispGammaCfg);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

UINT32 AmbaDSP_VoutDisplayControlGamma(const UINT8 VoutIdx, UINT8 Enable)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_vout_disp_gamma_ctrl_t DspVoutDispGammaCtrl = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspVoutDispGammaCtrl.VoutIdx = VoutIdx;
        DspVoutDispGammaCtrl.Enable = Enable;
        retcode = ioctl(dsp_fd, AMBADSP_VOUT_DISP_GAMMA_CTRL, &DspVoutDispGammaCtrl);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

UINT32 AmbaDSP_VoutVideoConfig(const UINT8 VoutIdx, const AMBA_DSP_VOUT_VIDEO_CFG_s *pConfig)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_vout_video_cfg_t DspVoutVideoCfg = {0};
    ULONG phys_addr;

    /* ioctl */
    if (pConfig == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspVoutVideoCfg.VoutIdx = VoutIdx;
            Rval = dsp_wrapper_osal_memcpy(&DspVoutVideoCfg.Cfg, pConfig, sizeof(AMBA_DSP_VOUT_VIDEO_CFG_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            /* virt2phys */
            Rval = dsp_wrapper_osal_virt2phys(DspVoutVideoCfg.Cfg.DefaultImgConfig.BaseAddrY, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspVoutVideoCfg.Cfg.DefaultImgConfig.BaseAddrY = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspVoutVideoCfg.Cfg.DefaultImgConfig.BaseAddrUV, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspVoutVideoCfg.Cfg.DefaultImgConfig.BaseAddrUV = phys_addr;

            retcode = ioctl(dsp_fd, AMBADSP_VOUT_VIDEO_CFG, &DspVoutVideoCfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VoutVideoCtrl(const UINT8 VoutIdx, UINT8 Enable, UINT8 SyncWithVin, UINT64 *pAttachedRawSeq)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_vout_video_ctrl_t DspVoutVideoCtrl = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspVoutVideoCtrl.VoutIdx = VoutIdx;
        DspVoutVideoCtrl.Enable = Enable;
        DspVoutVideoCtrl.SyncWithVin = SyncWithVin;
        if (0U == IS_SUCCESSOR_STATE()) {
            retcode = ioctl(dsp_fd, AMBADSP_VOUT_VIDEO_CTRL, &DspVoutVideoCtrl);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
        if (pAttachedRawSeq != NULL) {
            *pAttachedRawSeq = DspVoutVideoCtrl.AttachedRawSeq;
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VoutConfigMixerBinding(UINT8 NumVout, const UINT8 *pVoutIdx, const AMBA_DSP_VOUT_DATA_PATH_CFG_s *pConfig)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_vout_mixer_bind_cfg_t DspVoutMixerBindCfg = {0};

    /* ioctl */
    if (pConfig == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            if (NumVout > 0) {
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
                uint32_t i;
                for (i = 0U; i < NumVout; i++) {
                    if ((pVoutIdx != NULL) && (pConfig != NULL)) {
                        VoutDisplayOption[pVoutIdx[i]] = pConfig[i].DisplayOption;
                    }
                }
#endif
                DspVoutMixerBindCfg.NumVout = NumVout;
                Rval = dsp_wrapper_osal_memcpy(&DspVoutMixerBindCfg.VoutIdx[0U], pVoutIdx, sizeof(UINT8)*NumVout);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                Rval = dsp_wrapper_osal_memcpy(&DspVoutMixerBindCfg.PathCfg[0U], pConfig, sizeof(AMBA_DSP_VOUT_DATA_PATH_CFG_s)*NumVout);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                retcode = ioctl(dsp_fd, AMBADSP_VOUT_MIXER_BIND_CFG, &DspVoutMixerBindCfg);
                Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VoutDveConfig(const UINT8 VoutIdx, UINT8 DveMode)
{
    uint32_t Rval = DSP_ERR_NONE;
(void)VoutIdx;
(void)DveMode;
#if !defined (CONFIG_SOC_CV2FS) && !defined (CONFIG_SOC_CV22FS)
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_vout_drv_cfg_t DspVoutDrvCfg = {0};
    dsp_vout_dve_cfg_t DspVoutDveCfg = {0};
    ULONG DrvCfgAddr;
    uint32_t DrvCfgSize, DataSize;
    void *pVoid;

    /* ioctl */
    if (dsp_fd > 0) {
        if (VoutIdx == VOUT_IDX_B) {
            Rval = AmbaVout_GetTvEncAddr(&DrvCfgAddr, &DrvCfgSize);
        } else {
            Rval = DSP_ERR_0001;
        }

        if (Rval != DSP_ERR_NONE) {
            dsp_wrapper_osal_printU5("[ERR] AmbaDSP_VoutDveConfig() : Vout_GetTveAddr fail 0x%X", Rval, 0U, 0U, 0U, 0U);
        } else {
            DataSize = (uint32_t)sizeof(DspVoutDrvCfg.VoutDrvCfg.Data);
            if (DrvCfgSize > DataSize) {
                dsp_wrapper_osal_printU5("[ERR] AmbaDSP_VoutDveConfig() : Vout_GetTveAddr fail Data size exceed %d %d", DrvCfgSize, DataSize, 0U, 0U, 0U);
                Rval = DSP_ERR_0001;
            }
        }

        if (Rval == DSP_ERR_NONE) {
            DspVoutDrvCfg.VoutId = VoutIdx;
            DspVoutDrvCfg.Type = DSP_VOUT_DRV_CFG_TYPE_TVE;
            dsp_wrapper_osal_typecast(&pVoid, &DrvCfgAddr);
            Rval = dsp_wrapper_osal_memcpy(DspVoutDrvCfg.VoutDrvCfg.Data, pVoid, DrvCfgSize);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            retcode = ioctl(dsp_fd, AMBADSP_VOUT_DRV_CFG, &DspVoutDrvCfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        }

        DspVoutDveCfg.VoutIdx = VoutIdx;
        DspVoutDveCfg.DveMode = DveMode;
        retcode = ioctl(dsp_fd, AMBADSP_VOUT_DVE_CFG, &DspVoutDveCfg);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }
#endif
    return Rval;
}

UINT32 AmbaDSP_VoutDveCtrl(const UINT8 VoutIdx)
{
    uint32_t Rval = DSP_ERR_NONE;
(void)VoutIdx;
#if !defined (CONFIG_SOC_CV2FS) && !defined (CONFIG_SOC_CV22FS)
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();

    /* ioctl */
    if (dsp_fd > 0) {
        retcode = ioctl(dsp_fd, AMBADSP_VOUT_DVE_CTRL, &VoutIdx);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }
#endif
    return Rval;
}

static inline void LvCfg_FillLvYuvBuf(AMBA_DSP_LIVEVIEW_YUV_BUF_s* pYuvBuf, dsp_liveview_yuv_buf_t* pCfgYuvBuf)
{
    uint32_t Rval = DSP_ERR_NONE;
    uint16_t i;
    ULONG phys_addr;

    if (pYuvBuf != NULL) {
        pCfgYuvBuf->AllocType = pYuvBuf->AllocType;
        pCfgYuvBuf->BufNum = pYuvBuf->BufNum;
        Rval = dsp_wrapper_osal_memcpy(&pCfgYuvBuf->YuvBuf, &pYuvBuf->YuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        if (pYuvBuf->pYuvBufTbl != NULL) {
            /* virt2phys */
            for (i = 0U; i < pYuvBuf->BufNum; i++) {
                Rval = dsp_wrapper_osal_virt2phys(pYuvBuf->pYuvBufTbl[i], &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pCfgYuvBuf->YuvBufTbl[i] = phys_addr;
            }
        }
        /* virt2phys */
        if (pYuvBuf->YuvBuf.BaseAddrY != 0U) {
            Rval = dsp_wrapper_osal_virt2phys(pYuvBuf->YuvBuf.BaseAddrY, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            pCfgYuvBuf->YuvBuf.BaseAddrY= phys_addr;
        }
    }
}

UINT32 AmbaDSP_LiveviewConfig(UINT16 NumViewZone, const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pViewZoneCfg, UINT16 NumYUVStream, const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYUVStrmCfg)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    uint16_t i, j, k;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_liveview_cfg_t DspLiveviewCfg = {0};
    ULONG phys_addr;
    uint32_t CfgMask = 0U;
    uint8_t *pU8Val = NULL;
    dsp_liveview_viewzone_cfg_t *pCfgViewZone = NULL;
    dsp_liveview_strm_cfg_t *pCfgYuvStrm = NULL;

    /* ioctl */
    if ((pViewZoneCfg == NULL) || (pYUVStrmCfg == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            if (NumViewZone > 0U) {
                CfgMask |= 1U << DSP_LV_CFG_TYPE_VIEWZONE;
            }
            if (NumYUVStream > 0U) {
                CfgMask |= 1U << DSP_LV_CFG_TYPE_YUVSTRM;
            }

            pU8Val = DspLiveviewCfg.cfg.Data;

            for (i = 0U; i < NumViewZone; i++) {
                Rval = dsp_wrapper_osal_memset(&DspLiveviewCfg, 0, sizeof(dsp_liveview_cfg_t));
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspLiveviewCfg.CfgMask = (UINT8)CfgMask;
                DspLiveviewCfg.Type = DSP_LV_CFG_TYPE_VIEWZONE;
                DspLiveviewCfg.Index = i;
                DspLiveviewCfg.FreshNew = (i == 0U)? 1U: 0U;
                DspLiveviewCfg.TotalNum = NumViewZone;

                dsp_wrapper_osal_typecast(&pCfgViewZone, &pU8Val);
                pCfgViewZone->ViewZoneId = pViewZoneCfg[i].ViewZoneId;
                pCfgViewZone->VinSource = pViewZoneCfg[i].VinSource;
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
                ViewZoneSource[pViewZoneCfg[i].ViewZoneId] = pViewZoneCfg[i].VinSource;
#endif
                pCfgViewZone->VinId = pViewZoneCfg[i].VinId;
                pCfgViewZone->VinVirtChId = pViewZoneCfg[i].VinVirtChId;
                pCfgViewZone->IsVirtChan = pViewZoneCfg[i].IsVirtChan;
                Rval = dsp_wrapper_osal_memcpy(&pCfgViewZone->VinRoi, &pViewZoneCfg[i].VinRoi, sizeof(AMBA_DSP_WINDOW_s));
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pCfgViewZone->MainWidth = pViewZoneCfg[i].MainWidth;
                pCfgViewZone->MainHeight = pViewZoneCfg[i].MainHeight;
                pCfgViewZone->RotateFlip = pViewZoneCfg[i].RotateFlip;
                ViewZoneRotate[pViewZoneCfg[i].ViewZoneId] = pViewZoneCfg[i].RotateFlip;

                pCfgViewZone->HdrExposureNum = pViewZoneCfg[i].HdrExposureNum;
                pCfgViewZone->HdrBlendHieght = pViewZoneCfg[i].HdrBlendHieght;
                pCfgViewZone->MctfDisable = pViewZoneCfg[i].MctfDisable;
                pCfgViewZone->MctsDisable = pViewZoneCfg[i].MctsDisable;
                pCfgViewZone->LinearCE = pViewZoneCfg[i].LinearCE;
                Rval = dsp_wrapper_osal_memcpy(&pCfgViewZone->Pyramid, &pViewZoneCfg[i].Pyramid, sizeof(AMBA_DSP_LIVEVIEW_PYRAMID_s));
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                LvCfg_FillLvYuvBuf(pViewZoneCfg[i].pPyramidBuf, &pCfgViewZone->PyramidBuf);
                Rval = dsp_wrapper_osal_memcpy(&pCfgViewZone->LaneDetect, &pViewZoneCfg[i].LaneDetect, sizeof(AMBA_DSP_LIVEVIEW_LANE_DETECT_s));
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                LvCfg_FillLvYuvBuf(pViewZoneCfg[i].pLaneDetectBuf, &pCfgViewZone->LaneDetectBuf);
                pCfgViewZone->VinDecimation = pViewZoneCfg[i].VinDecimation;
                pCfgViewZone->DramUtilityPipe = pViewZoneCfg[i].DramUtilityPipe;
                ViewZonePipe[pViewZoneCfg[i].ViewZoneId] = pViewZoneCfg[i].DramUtilityPipe;

                pCfgViewZone->InputMuxSelCfg = pViewZoneCfg[i].InputMuxSelCfg;

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
                LvCfg_FillLvYuvBuf(pViewZoneCfg[i].pMainY12Buf, &pCfgViewZone->MainY12Buf);
#endif
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
                LvCfg_FillLvYuvBuf(pViewZoneCfg[i].pMainIrBuf, &pCfgViewZone->MainIrBuf);
                pCfgViewZone->ExtraHorWarp = pViewZoneCfg[i].ExtraHorWarp;
#endif

#if !defined (CONFIG_SOC_CV2FS) && !defined (CONFIG_SOC_CV22FS) && !defined (CONFIG_SOC_CV5) && !defined (CONFIG_SOC_CV52) && !defined (CONFIG_SOC_CV7)
                pCfgViewZone->UpstreamSource = pViewZoneCfg[i].UpstreamSource;
                LvCfg_FillLvYuvBuf(pViewZoneCfg[i].pIntMainBuf, &pCfgViewZone->IntMainBuf);
#endif

                if (0U == IS_SUCCESSOR_STATE()) {
                    retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_CFG, &DspLiveviewCfg);
                    Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
                }
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            }

            for (i = 0U; i < NumYUVStream; i++) {
                Rval = dsp_wrapper_osal_memset(&DspLiveviewCfg, 0, sizeof(dsp_liveview_cfg_t));
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspLiveviewCfg.CfgMask = (UINT8)CfgMask;
                DspLiveviewCfg.Type = DSP_LV_CFG_TYPE_YUVSTRM;
                DspLiveviewCfg.Index = i;
                DspLiveviewCfg.FreshNew = (i == 0U)? 1U: 0U;
                DspLiveviewCfg.TotalNum = NumYUVStream;

                dsp_wrapper_osal_typecast(&pCfgYuvStrm, &pU8Val);
                pCfgYuvStrm->StreamId = pYUVStrmCfg[i].StreamId;
                pCfgYuvStrm->Purpose = pYUVStrmCfg[i].Purpose;
                pCfgYuvStrm->DestVout = pYUVStrmCfg[i].DestVout;
                pCfgYuvStrm->DestEnc = (UINT16)pYUVStrmCfg[i].DestEnc;
                pCfgYuvStrm->Width = pYUVStrmCfg[i].Width;
                pCfgYuvStrm->Height = pYUVStrmCfg[i].Height;
                pCfgYuvStrm->MaxWidth = pYUVStrmCfg[i].MaxWidth;
                pCfgYuvStrm->MaxHeight = pYUVStrmCfg[i].MaxHeight;
                {
                    pCfgYuvStrm->StreamBuf.AllocType = pYUVStrmCfg[i].StreamBuf.AllocType;
                    pCfgYuvStrm->StreamBuf.BufNum = pYUVStrmCfg[i].StreamBuf.BufNum;
                    Rval = dsp_wrapper_osal_memcpy(&pCfgYuvStrm->StreamBuf.YuvBuf, &pYUVStrmCfg[i].StreamBuf.YuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

                    /* virt2phys */
                    Rval = dsp_wrapper_osal_virt2phys(pCfgYuvStrm->StreamBuf.YuvBuf.BaseAddrY, &phys_addr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pCfgYuvStrm->StreamBuf.YuvBuf.BaseAddrY = phys_addr;

                    Rval = dsp_wrapper_osal_virt2phys(pCfgYuvStrm->StreamBuf.YuvBuf.BaseAddrUV, &phys_addr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pCfgYuvStrm->StreamBuf.YuvBuf.BaseAddrUV = phys_addr;

                    /* virt2phys */
                    if (pYUVStrmCfg[i].StreamBuf.pYuvBufTbl != NULL) {
                        for (j = 0U; j < pYUVStrmCfg[i].StreamBuf.BufNum; j++) {
                            Rval = dsp_wrapper_osal_virt2phys(pYUVStrmCfg[i].StreamBuf.pYuvBufTbl[j], &phys_addr);
                            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                            pCfgYuvStrm->StreamBuf.YuvBufTbl[j] = phys_addr;
                        }
                    }
                }

                pCfgYuvStrm->NumChan = pYUVStrmCfg[i].NumChan;
                {
                    /* copy ChanCfg */
                    for (j = 0U; j < AMBA_DSP_MAX_YUVSTRM_VIEW_NUM; j++) {
                        pCfgYuvStrm->ChanCfg[j].ViewZoneId = pYUVStrmCfg[i].pChanCfg[j].ViewZoneId;
                        Rval = dsp_wrapper_osal_memcpy(&pCfgYuvStrm->ChanCfg[j].ROI, &pYUVStrmCfg[i].pChanCfg[j].ROI, sizeof(AMBA_DSP_WINDOW_s));
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                        Rval = dsp_wrapper_osal_memcpy(&pCfgYuvStrm->ChanCfg[j].Window, &pYUVStrmCfg[i].pChanCfg[j].Window, sizeof(AMBA_DSP_WINDOW_s));
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                        pCfgYuvStrm->ChanCfg[j].RotateFlip = pYUVStrmCfg[i].pChanCfg[j].RotateFlip;
                        pCfgYuvStrm->ChanCfg[j].BlendNum = pYUVStrmCfg[i].pChanCfg[j].BlendNum;

                        /* virt2phys */
                        if (pYUVStrmCfg[i].pChanCfg[j].LumaAlphaTable != 0U) {
                            AMBA_DSP_BUF_s *pBldBuf;
                            UINT8 BldNum;

                            dsp_wrapper_osal_typecast(&pBldBuf, &pYUVStrmCfg[i].pChanCfg[j].LumaAlphaTable);
                            BldNum = (pYUVStrmCfg[i].pChanCfg[j].BlendNum < AMBA_DSP_MAX_YUVSTRM_BLD_NUM)? pYUVStrmCfg[i].pChanCfg[j].BlendNum: AMBA_DSP_MAX_YUVSTRM_BLD_NUM;
                            Rval = dsp_wrapper_osal_memcpy(&pCfgYuvStrm->ChanCfg[j].BldBuf[0], pBldBuf, sizeof(AMBA_DSP_BUF_s)*BldNum);
                            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

                            for (k = 0U; k < BldNum; k++) {
                                Rval = dsp_wrapper_osal_virt2phys(pBldBuf[k].BaseAddr, &phys_addr);
                                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                                pCfgYuvStrm->ChanCfg[j].BldBuf[k].BaseAddr = phys_addr;
                            }
                        }
                    }
                }

                pCfgYuvStrm->MaxChanBitMask = pYUVStrmCfg[i].MaxChanBitMask;
                pCfgYuvStrm->MaxChanNum = pYUVStrmCfg[i].MaxChanNum;
                Rval = dsp_wrapper_osal_memcpy(pCfgYuvStrm->MaxChanWin, pYUVStrmCfg[i].pMaxChanWin, sizeof(AMBA_DSP_WINDOW_DIMENSION_s)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pCfgYuvStrm->DestDeciRate = pYUVStrmCfg[i].DestDeciRate;
#if !defined (CONFIG_SOC_CV2FS) && !defined (CONFIG_SOC_CV22FS)
                pCfgYuvStrm->OptimizeOption = pYUVStrmCfg[i].OptimizeOption;
#endif

                if (0U == IS_SUCCESSOR_STATE()) {
                    retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_CFG, &DspLiveviewCfg);
                    Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
                }
            }
        }
    }

    return Rval;
}

#ifndef IK_HORIZONTAL_FLIP
    #define IK_HORIZONTAL_FLIP          (0x1UL)
#endif
#ifndef IK_VERTICAL_FLIP
    #define IK_VERTICAL_FLIP            (0x2UL)
#endif
#ifndef IK_FLIP_RAW_V
    #define IK_FLIP_RAW_V               (IK_VERTICAL_FLIP)
#endif
#ifndef IK_FLIP_YUV_H
    #define IK_FLIP_YUV_H               (IK_HORIZONTAL_FLIP<<16UL)
#endif
static inline void dsp_wrap_fill_ik_flip(UINT16 ViewZoneId, UINT32 *pFlipMode)
{
    if ((ViewZoneRotate[ViewZoneId] == AMBA_DSP_ROTATE_0_HORZ_FLIP) ||
        (ViewZoneRotate[ViewZoneId] == AMBA_DSP_ROTATE_180) ||
        (ViewZoneRotate[ViewZoneId] == AMBA_DSP_ROTATE_270) ||
        (ViewZoneRotate[ViewZoneId] == AMBA_DSP_ROTATE_270_VERT_FLIP)) {
        *pFlipMode |= (UINT32)IK_FLIP_YUV_H;
    }
    if ((ViewZoneRotate[ViewZoneId] == AMBA_DSP_ROTATE_90_VERT_FLIP) ||
        (ViewZoneRotate[ViewZoneId] == AMBA_DSP_ROTATE_180) ||
        (ViewZoneRotate[ViewZoneId] == AMBA_DSP_ROTATE_180_HORZ_FLIP) ||
        (ViewZoneRotate[ViewZoneId] == AMBA_DSP_ROTATE_270)) {
        *pFlipMode |= (UINT32)IK_FLIP_RAW_V;
    }
}

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
static inline void dsp_wrap_fill_ik_stitch(UINT16 ViewZoneId, AMBA_IK_STITCH_INFO_s *pStitchInfo)
{
    if (pStitchInfo != NULL) {
        if ((ViewZoneSliceCfg[ViewZoneId].SliceNumCol > 1U) ||
            (ViewZoneSliceCfg[ViewZoneId].SliceNumRow > 1U) ||
            (IsLvTileMode == 1U)) {
            pStitchInfo->Enable = 1U;
            pStitchInfo->TileNum_x = ViewZoneSliceCfg[ViewZoneId].SliceNumCol;
            pStitchInfo->TileNum_y = ViewZoneSliceCfg[ViewZoneId].SliceNumRow;
        } else {
            pStitchInfo->Enable = 0U;
            pStitchInfo->TileNum_x = 1U;
            pStitchInfo->TileNum_y = 1U;
        }
    }
}
#endif

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
static inline UINT32 dsp_wrap_lv_ctrl_vz_info_collect(UINT16 NumViewZone, const UINT16 *pViewZoneId, const UINT8 *pEnable)
{
    int32_t retcode = 0;
    UINT16 i;
    UINT32 Rval = DSP_ERR_NONE;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_liveview_slice_cfg_t DspLvSliceCfg = {0};

    IsLvTileMode = 0U;
    for (i = 0U; i < NumViewZone; i++) {
        if (pEnable[i] == (UINT8)0U) {
            continue;
        }

        /* Query slice config */
        if ((Rval == DSP_ERR_NONE) &&
            ((ViewZonePipe[pViewZoneId[i]] != (UINT8)DSP_DRAM_PIPE_RAW_ONLY) &&
             (ViewZoneSource[pViewZoneId[i]] != (UINT8)DSP_LV_SOURCE_MEM_DEC) &&
             (ViewZoneSource[pViewZoneId[i]] != (UINT8)DSP_LV_SOURCE_MEM_RECON))) {
            (void)dsp_wrapper_osal_memset(&ViewZoneSliceCfg[pViewZoneId[i]], 0, sizeof(AMBA_DSP_LIVEVIEW_SLICE_CFG_s));

            DspLvSliceCfg.ViewZoneId = pViewZoneId[i];
            retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_SLICE_CFG_CALC, &DspLvSliceCfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            if (Rval == DSP_ERR_NONE) {
                //dsp_wrapper_osal_printU5("AmbaDSP_LiveviewCtrl %u vz:%u Rval:0x%X", __LINE__, pViewZoneId[i], Rval, 0, 0);
                //dsp_wrapper_osal_printU5("LvSliceCfg SliceNum:%u %u EncSyncOpt:%u",
                //        ViewZoneSliceCfg[pViewZoneId[i]].SliceNumCol, ViewZoneSliceCfg[pViewZoneId[i]].SliceNumRow, ViewZoneSliceCfg[pViewZoneId[i]].EncSyncOpt, 0, 0);
                //dsp_wrapper_osal_printU5("LvSliceCfg Warp:%u %u %u VinDragLine:%u",
                //        ViewZoneSliceCfg[pViewZoneId[i]].WarpLumaWaitLine, ViewZoneSliceCfg[pViewZoneId[i]].WarpChromaWaitLine, ViewZoneSliceCfg[pViewZoneId[i]].WarpOverLap, ViewZoneSliceCfg[pViewZoneId[i]].VinDragLine, 0);
                dsp_wrapper_osal_memcpy(&ViewZoneSliceCfg[pViewZoneId[i]], &DspLvSliceCfg.LvSliceCfg, sizeof(AMBA_DSP_LIVEVIEW_SLICE_CFG_s));
            }

            if ((ViewZoneSliceCfg[pViewZoneId[i]].SliceNumCol > 1U) ||
                (ViewZoneSliceCfg[pViewZoneId[i]].SliceNumRow > 1U)) {
                IsLvTileMode = 1U;
            }
        }
    }

    return Rval;
}
#endif

static inline void DSPWrap_FillHdrRawOffset(dsp_liveview_ik_drv_cfg_t *pDspLvIkDrvCfg,
                                            const UINT16 ViewZoneId,
                                            const AMBA_IK_MODE_CFG_s *pImgMode,
                                            const AMBA_IK_ABILITY_s *pIkAbility,
                                            const uint32_t IsoId,
                                            const ULONG IsoAddr)
{
    uint32_t Rval = DSP_ERR_NONE;
    uint8_t *pU8Val;
    dsp_ik_drv_cfg_dol_offset_t *pCfgDolOfst;
    uint16_t j;
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
    AMBA_IK_QUERY_FRAME_INFO_s FrmInfo = {0};
#else
    AMBA_IK_HDR_RAW_INFO_s HdrRawInfo = {0};
    void *pVoid;
#endif

    (void)pImgMode;
    (void)IsoId;
    (void)IsoAddr;

    Rval = dsp_wrapper_osal_memset(pDspLvIkDrvCfg, 0, sizeof(dsp_liveview_ik_drv_cfg_t));
    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

    pDspLvIkDrvCfg->ViewZoneId = ViewZoneId;
    pDspLvIkDrvCfg->Type = DSP_IK_DRV_CFG_TYPE_DOL_OFST;

    pU8Val = pDspLvIkDrvCfg->IkDrvCfg.Data;
    dsp_wrapper_osal_typecast(&pCfgDolOfst, &pU8Val);
    pCfgDolOfst->Pipe = DSP_IK_PIPE_VDO;

    if ((pIkAbility->VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_2) ||
        (pIkAbility->VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_3)) {
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
        Rval = AmbaIK_QueryFrameInfo(pImgMode, IsoId, &FrmInfo);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
#else
        dsp_wrapper_osal_typecast(&pVoid, &IsoAddr);
        Rval = AmbaIK_GetIsoHdrRawOffest(pVoid, &HdrRawInfo);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
#endif
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
        for (j = 0U; j < IK_MAX_HDR_EXPOSURE_NUM; j++) {
            pCfgDolOfst->Y[j] = FrmInfo.HdrRawInfo.YOffset[j];
        }
#else
        for (j = 0U; j < MAX_IK_MAX_HDR_EXPOSURE_NUM; j++) {
            pCfgDolOfst->Y[j] = HdrRawInfo.YOffset[j];
        }
#endif
    } else {
        for (j = 0U; j < DSP_IK_DOL_OFST_NUM; j++) {
            pCfgDolOfst->Y[j] = 0U;
        }
    }
}

UINT32 AmbaDSP_LiveviewCtrl(UINT16 NumViewZone, const UINT16 *pViewZoneId, const UINT8 *pEnable)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    uint32_t i;
    dsp_liveview_ctrl_t DspLiveviewCtrl = {0};
    dsp_liveview_ik_drv_cfg_t DspLvIkDrvCfg;
    AMBA_IK_MODE_CFG_s ImgMode;
    uint32_t FlipMode;
    AMBA_IK_ABILITY_s IkAbility = {0};
    AMBA_IK_VIN_SENSOR_INFO_s VinSensorInfo = {0};
    AMBA_IK_EXECUTE_CONTAINER_s CfgExecInfo = {0};
    uint32_t IsoId;
    ULONG IsoAddr;
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
    AMBA_IK_QUERY_FRAME_INFO_s FrmInfo = {0};
    AMBA_IK_STITCH_INFO_s StitchInfo = {0};
#else
    AMBA_IK_HDR_RAW_INFO_s HdrRawInfo = {0};
    void *pVoid;
#endif
    uint8_t *pU8Val;
    dsp_ik_drv_cfg_sensor_info_t *pCfgSensorInfo;
    dsp_ik_drv_cfg_ability_t *pCfgAbility;
    dsp_ik_drv_cfg_iso_t *pCfgIso;
    dsp_ik_drv_cfg_dol_offset_t *pCfgDolOfst;
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
    dsp_ik_drv_cfg_ext_raw_cmpr_info_t *pCfgExtRawCmpr;
#endif
    ULONG phys_addr;
    uint32_t j;

    /* ioctl */
    if ((pViewZoneId == NULL) || (pEnable == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
            Rval = dsp_wrap_lv_ctrl_vz_info_collect(NumViewZone, pViewZoneId, pEnable);
#endif

            /* Issue Ik relative APIs here */
            for (i = 0U; i < NumViewZone; i++) {
                if (pEnable[i] == (UINT8)0U) {
                    continue;
                }

                ImgMode.ContextId = ViewZoneIkCtxId[pViewZoneId[i]];

                pU8Val = DspLvIkDrvCfg.IkDrvCfg.Data;

                Rval = AmbaIK_GetContextAbility(&ImgMode, &IkAbility);
                if (Rval != DSP_ERR_NONE) {
                    dsp_wrapper_osal_printU5("Ctx[%d] GetContextAbility Fail", ViewZoneIkCtxId[pViewZoneId[i]], 0U, 0U, 0U, 0U);
                }
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
                if (IkAbility.VideoPipe != AMBA_IK_VIDEO_Y2Y) {
#else
                    if ((ViewZonePipe[pViewZoneId[i]] == (UINT8)DSP_DRAM_PIPE_MIPI_RAW) ||
                        (ViewZonePipe[pViewZoneId[i]] == (UINT8)DSP_DRAM_PIPE_RAW_PROC) ||
                        ((IkAbility.VideoPipe != AMBA_IK_VIDEO_Y2Y) && (IkAbility.VideoPipe != AMBA_IK_VIDEO_Y2Y_MIPI))) {
#endif
                    Rval = dsp_wrapper_osal_memset(&DspLvIkDrvCfg, 0, sizeof(dsp_liveview_ik_drv_cfg_t));
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    DspLvIkDrvCfg.ViewZoneId = pViewZoneId[i];
                    DspLvIkDrvCfg.Type = DSP_IK_DRV_CFG_TYPE_SNSR_INFO;
                    Rval = AmbaIK_GetVinSensorInfo(&ImgMode, &VinSensorInfo);
                    if (Rval == DSP_ERR_NONE) {
                        dsp_wrapper_osal_typecast(&pCfgSensorInfo, &pU8Val);
                        pCfgSensorInfo->Pipe = DSP_IK_PIPE_VDO;
                        pCfgSensorInfo->SensorMode = VinSensorInfo.SensorMode;
                        pCfgSensorInfo->Compression = VinSensorInfo.Compression;
                        retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_IK_DRV_CFG, &DspLvIkDrvCfg);
                        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    }
                }

                if (ViewZonePipe[pViewZoneId[i]] != (UINT8)DSP_DRAM_PIPE_RAW_ONLY) {
                    FlipMode = 0U;
                    dsp_wrap_fill_ik_flip(pViewZoneId[i], &FlipMode);
                    if (FlipMode != 0U) {
                        Rval = AmbaIK_SetFlipMode(&ImgMode, FlipMode);
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    }

                    Rval = dsp_wrapper_osal_memset(&DspLvIkDrvCfg, 0, sizeof(dsp_liveview_ik_drv_cfg_t));
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    DspLvIkDrvCfg.ViewZoneId = pViewZoneId[i];
                    DspLvIkDrvCfg.Type = DSP_IK_DRV_CFG_TYPE_ABILITY;
                    dsp_wrapper_osal_typecast(&pCfgAbility, &pU8Val);
                    pCfgAbility->Pipe = IkAbility.Pipe;
#if !defined (CONFIG_SOC_CV2FS) && !defined (CONFIG_SOC_CV22FS)
                    pCfgAbility->StlPipe = (IkAbility.StillPipe == AMBA_IK_STILL_HISO)? DSP_IK_STL_PIPE_HI: DSP_IK_STL_PIPE_LI;
#else
                    pCfgAbility->StlPipe = DSP_IK_STL_PIPE_LI;
#endif
                    retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_IK_DRV_CFG, &DspLvIkDrvCfg);
                    Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
                    /* Query stitch */
                    if ((ViewZoneSource[pViewZoneId[i]] != (UINT8)DSP_LV_SOURCE_MEM_DEC) &&
                        (ViewZoneSource[pViewZoneId[i]] != (UINT8)DSP_LV_SOURCE_MEM_RECON)){
                        dsp_wrap_fill_ik_stitch(pViewZoneId[i], &StitchInfo);
                        if (0U == IS_SUCCESSOR_STATE()) {
                            Rval = AmbaIK_SetStitchingInfo(&ImgMode, &StitchInfo);
                        }
                        if (Rval != DSP_ERR_NONE) {
                            dsp_wrapper_osal_printU5("Ctx[%d] SetStitchingInfo Fail(0x%X)", ImgMode.ContextId, Rval, 0U, 0U, 0U);
                        }
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
                        else {
                            if (IkAbility.Pipe != AMBA_IK_PIPE_STILL) {
                                AMBA_IK_BURST_TILE_s BurstTile = {0};

                                if ((StitchInfo.Enable == 1U) &&
                                    (StitchInfo.TileNum_x > 1U) &&
                                    (ViewZonePipe[pViewZoneId[i]] != DSP_DRAM_PIPE_EFCY)) {
                                    BurstTile.Enable = 1U;
                                } else {
                                    BurstTile.Enable = 0U;
                                }

                                if (0U == IS_SUCCESSOR_STATE()) {
                                    Rval = AmbaIK_SetBurstTile(&ImgMode, &BurstTile);
                                }
                                if (Rval != DSP_ERR_NONE) {
                                    dsp_wrapper_osal_printU5("Ctx[%d] SetBurstTile Fail(0x%X)", ImgMode.ContextId, Rval, 0U, 0U, 0U);
                                }

                                {
                                    AMBA_IK_OVERLAP_X_s OverLap = {0};

                                    if ((StitchInfo.Enable == 1U) &&
                                        (StitchInfo.TileNum_x > 1U)) {
                                        OverLap.OverlapX = ViewZoneMaxHorWarpComp[pViewZoneId[i]];
                                    } else {
                                        OverLap.OverlapX = 0U;
                                    }
                                    if (0U == IS_SUCCESSOR_STATE()) {
                                        Rval = AmbaIK_SetOverlapX(&ImgMode, &OverLap);
                                    }
                                }

                            }
                        }
#endif
                    }
#endif

                    if (0U == IS_SUCCESSOR_STATE()) {
                        Rval = AmbaIK_ExecuteConfig(&ImgMode, &CfgExecInfo);
                        if (Rval != DSP_ERR_NONE) {
                            dsp_wrapper_osal_printU5("!!! IK Exec[%d] Fail(0x%X)", ImgMode.ContextId, Rval, 0U, 0U, 0U);
                        } else {
                            if (CfgExecInfo.pIsoCfg == NULL) {
                                dsp_wrapper_osal_printU5("!!! IK Exec[%d] Null IsoCfg", ImgMode.ContextId, 0U, 0U, 0U, 0U);
                            } else {
                                IsoId = CfgExecInfo.IkId;
                                dsp_wrapper_osal_typecast(&IsoAddr, &CfgExecInfo.pIsoCfg);
                                //dsp_wrapper_osal_printU5("!!! IK Exec[%d] IsoAddr V 0x%X", pViewZoneId[i], IsoAddr, 0U, 0U, 0U);

                                Rval = dsp_wrapper_osal_memset(&DspLvIkDrvCfg, 0, sizeof(dsp_liveview_ik_drv_cfg_t));
                                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                                DspLvIkDrvCfg.ViewZoneId = pViewZoneId[i];
                                DspLvIkDrvCfg.Type = DSP_IK_DRV_CFG_TYPE_ISO;
                                dsp_wrapper_osal_typecast(&pCfgIso, &pU8Val);
                                pCfgIso->Id = IsoId;
                                Rval = dsp_wrapper_osal_virt2phys(IsoAddr, &phys_addr);
                                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                                pCfgIso->Addr = phys_addr;
                                retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_IK_DRV_CFG, &DspLvIkDrvCfg);
                                Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
                                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

                                DSPWrap_FillHdrRawOffset(&DspLvIkDrvCfg,
                                                         pViewZoneId[i],
                                                         &ImgMode,
                                                         &IkAbility,
                                                         IsoId,
                                                         IsoAddr);
                                retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_IK_DRV_CFG, &DspLvIkDrvCfg);
                                Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
                                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
                                Rval = dsp_wrapper_osal_memset(&DspLvIkDrvCfg, 0, sizeof(dsp_liveview_ik_drv_cfg_t));
                                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

                                DspLvIkDrvCfg.ViewZoneId = pViewZoneId[i];
                                DspLvIkDrvCfg.Type = DSP_IK_DRV_CFG_TYPE_EXT_RAW_CMPR;
                                dsp_wrapper_osal_typecast(&pCfgExtRawCmpr, &pU8Val);
                                pCfgExtRawCmpr->Pipe = DSP_IK_PIPE_VDO;
                                if ((IkAbility.VideoPipe != AMBA_IK_VIDEO_Y2Y) &&
                                    (IkAbility.StillPipe != AMBA_IK_STILL_HISO)) {
                                    Rval = AmbaIK_GetExtRawOutMode(&ImgMode, &pCfgExtRawCmpr->Compression);
                                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                                } else {
                                    pCfgExtRawCmpr->Compression = 0U;
                                }
                                Rval = ioctl(dsp_fd, AMBADSP_LIVEVIEW_IK_DRV_CFG, &DspLvIkDrvCfg);
#endif
                            }
                        }
                    }
                }
            }

            DspLiveviewCtrl.NumViewZone = NumViewZone;
            Rval = dsp_wrapper_osal_memcpy(DspLiveviewCtrl.ViewZoneId, pViewZoneId, sizeof(UINT16)*NumViewZone);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(DspLiveviewCtrl.Enable, pEnable, sizeof(UINT8)*NumViewZone);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            if (0U == IS_SUCCESSOR_STATE()) {
                retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_CTRL, &DspLiveviewCtrl);
                Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_LiveviewUpdateConfig(UINT16 NumYuvStream, const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYuvStrmCfg, UINT64 *pAttachedRawSeq)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    uint32_t i, j, k;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_liveview_update_cfg_t DspLiveviewUpdateCfg = {0};
    ULONG phys_addr;

    /* ioctl */
    if (pYuvStrmCfg != NULL) {
        if (dsp_fd > 0) {
            for (i = 0U; i < NumYuvStream; i++) {
                Rval = dsp_wrapper_osal_memset(&DspLiveviewUpdateCfg, 0, sizeof(dsp_liveview_update_cfg_t));
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                if (i == 0U) {
                    DspLiveviewUpdateCfg.FreshNew = 1U;
                }
                DspLiveviewUpdateCfg.Index = i;
                DspLiveviewUpdateCfg.TotalNum = NumYuvStream;

                DspLiveviewUpdateCfg.YuvStrmCfg.StreamId = pYuvStrmCfg[i].StreamId;
                DspLiveviewUpdateCfg.YuvStrmCfg.Purpose = pYuvStrmCfg[i].Purpose;
                DspLiveviewUpdateCfg.YuvStrmCfg.DestVout = pYuvStrmCfg[i].DestVout;
                DspLiveviewUpdateCfg.YuvStrmCfg.DestEnc = (UINT16)pYuvStrmCfg[i].DestEnc;
                DspLiveviewUpdateCfg.YuvStrmCfg.Width = pYuvStrmCfg[i].Width;
                DspLiveviewUpdateCfg.YuvStrmCfg.Height = pYuvStrmCfg[i].Height;
                DspLiveviewUpdateCfg.YuvStrmCfg.MaxWidth = pYuvStrmCfg[i].MaxWidth;
                DspLiveviewUpdateCfg.YuvStrmCfg.MaxHeight = pYuvStrmCfg[i].MaxHeight;
                {
                    DspLiveviewUpdateCfg.YuvStrmCfg.StreamBuf.AllocType = pYuvStrmCfg[i].StreamBuf.AllocType;
                    DspLiveviewUpdateCfg.YuvStrmCfg.StreamBuf.BufNum = pYuvStrmCfg[i].StreamBuf.BufNum;
                    Rval = dsp_wrapper_osal_memcpy(&DspLiveviewUpdateCfg.YuvStrmCfg.StreamBuf.YuvBuf, &pYuvStrmCfg[i].StreamBuf.YuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

                    /* virt2phys */
                    Rval = dsp_wrapper_osal_virt2phys(DspLiveviewUpdateCfg.YuvStrmCfg.StreamBuf.YuvBuf.BaseAddrY, &phys_addr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    DspLiveviewUpdateCfg.YuvStrmCfg.StreamBuf.YuvBuf.BaseAddrY = phys_addr;

                    Rval = dsp_wrapper_osal_virt2phys(DspLiveviewUpdateCfg.YuvStrmCfg.StreamBuf.YuvBuf.BaseAddrUV, &phys_addr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    DspLiveviewUpdateCfg.YuvStrmCfg.StreamBuf.YuvBuf.BaseAddrUV = phys_addr;

                    /* virt2phys */
                    for (j = 0U; j < pYuvStrmCfg[i].StreamBuf.BufNum; j++) {
                        if (pYuvStrmCfg[i].StreamBuf.pYuvBufTbl != NULL) {
                            Rval = dsp_wrapper_osal_virt2phys(pYuvStrmCfg[i].StreamBuf.pYuvBufTbl[j], &phys_addr);
                            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                            DspLiveviewUpdateCfg.YuvStrmCfg.StreamBuf.YuvBufTbl[j] = phys_addr;
                        }
                    }
                }
                DspLiveviewUpdateCfg.YuvStrmCfg.NumChan = pYuvStrmCfg[i].NumChan;
                /* copy ChanCfg */
                for (j = 0U; j < AMBA_DSP_MAX_YUVSTRM_VIEW_NUM; j++) {
                    DspLiveviewUpdateCfg.YuvStrmCfg.ChanCfg[j].ViewZoneId = pYuvStrmCfg[i].pChanCfg[j].ViewZoneId;
                    Rval = dsp_wrapper_osal_memcpy(&DspLiveviewUpdateCfg.YuvStrmCfg.ChanCfg[j].ROI, &pYuvStrmCfg[i].pChanCfg[j].ROI, sizeof(AMBA_DSP_WINDOW_s));
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    Rval = dsp_wrapper_osal_memcpy(&DspLiveviewUpdateCfg.YuvStrmCfg.ChanCfg[j].Window, &pYuvStrmCfg[i].pChanCfg[j].Window, sizeof(AMBA_DSP_WINDOW_s));
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    DspLiveviewUpdateCfg.YuvStrmCfg.ChanCfg[j].RotateFlip = pYuvStrmCfg[i].pChanCfg[j].RotateFlip;
                    DspLiveviewUpdateCfg.YuvStrmCfg.ChanCfg[j].BlendNum = pYuvStrmCfg[i].pChanCfg[j].BlendNum;

                    /* virt2phys */
                    if (pYuvStrmCfg[i].pChanCfg[j].LumaAlphaTable != 0U) {
                        AMBA_DSP_BUF_s *pBldBuf;
                        UINT8 BldNum;

                        dsp_wrapper_osal_typecast(&pBldBuf, &pYuvStrmCfg[i].pChanCfg[j].LumaAlphaTable);
                        BldNum = (pYuvStrmCfg[i].pChanCfg[j].BlendNum < AMBA_DSP_MAX_YUVSTRM_BLD_NUM)? pYuvStrmCfg[i].pChanCfg[j].BlendNum: AMBA_DSP_MAX_YUVSTRM_BLD_NUM;
                        Rval = dsp_wrapper_osal_memcpy(&DspLiveviewUpdateCfg.YuvStrmCfg.ChanCfg[j].BldBuf[0], pBldBuf, sizeof(AMBA_DSP_BUF_s)*BldNum);
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

                        for (k = 0U; k < BldNum; k++) {
                            Rval = dsp_wrapper_osal_virt2phys(pBldBuf[k].BaseAddr, &phys_addr);
                            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                            DspLiveviewUpdateCfg.YuvStrmCfg.ChanCfg[j].BldBuf[k].BaseAddr = phys_addr;
                        }
                    }
                }

                DspLiveviewUpdateCfg.YuvStrmCfg.MaxChanBitMask = pYuvStrmCfg[i].MaxChanBitMask;
                DspLiveviewUpdateCfg.YuvStrmCfg.MaxChanNum = pYuvStrmCfg[i].MaxChanNum;
                if (pYuvStrmCfg[i].pMaxChanWin != NULL) {
                    Rval = dsp_wrapper_osal_memcpy(DspLiveviewUpdateCfg.YuvStrmCfg.MaxChanWin, pYuvStrmCfg[i].pMaxChanWin, sizeof(AMBA_DSP_WINDOW_DIMENSION_s)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                }
                DspLiveviewUpdateCfg.YuvStrmCfg.DestDeciRate = pYuvStrmCfg[i].DestDeciRate;
#if !defined (CONFIG_SOC_CV2FS) && !defined (CONFIG_SOC_CV22FS)
                DspLiveviewUpdateCfg.YuvStrmCfg.OptimizeOption = pYuvStrmCfg[i].OptimizeOption;
#endif

                retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_UPDATE_CFG, &DspLiveviewUpdateCfg);
                Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            }
            if (pAttachedRawSeq != NULL) {
                *pAttachedRawSeq = DspLiveviewUpdateCfg.AttachedRawSeq;
            }
        }
    } else {
        Rval = DSP_ERR_0000;
    }

    return Rval;
}

UINT32 AmbaDSP_LiveviewUpdateIsoCfg(UINT16 NumViewZone, const AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s *pIsoCfgCtrl, UINT64 *pAttachedRawSeq)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_liveview_update_isocfg_t DspLiveviewUpdateIsocfg = {0};
    uint32_t i, j;
    ULONG phys_addr;
    dsp_liveview_ik_drv_cfg_t DspLvIkDrvCfg;
    AMBA_IK_MODE_CFG_s ImgMode;
    AMBA_IK_ABILITY_s IkAbility = {0};
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
    AMBA_IK_QUERY_FRAME_INFO_s FrmInfo = {0};
#else
    void *pVoid;
    AMBA_IK_HDR_RAW_INFO_s HdrRawInfo = {0};
#endif
    uint8_t *pU8Val;
    dsp_ik_drv_cfg_dol_offset_t *pCfgDolOfst;

    /* ioctl */
    if (pIsoCfgCtrl == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            /* Issue IK realtive APIs */
            for (i = 0U; i < NumViewZone; i++) {
                if (ViewZonePipe[pIsoCfgCtrl[i].ViewZoneId] != (UINT8)DSP_DRAM_PIPE_RAW_ONLY) {
                    if (pIsoCfgCtrl[i].CfgAddress > 0U) {
                        ImgMode.ContextId = pIsoCfgCtrl[i].CtxIndex;
                        (void)AmbaIK_GetContextAbility(&ImgMode, &IkAbility);

                        pU8Val = DspLvIkDrvCfg.IkDrvCfg.Data;
                        DspLvIkDrvCfg.ViewZoneId = pIsoCfgCtrl[i].ViewZoneId;
                        DspLvIkDrvCfg.Type = DSP_IK_DRV_CFG_TYPE_DOL_OFST;
                        dsp_wrapper_osal_typecast(&pCfgDolOfst, &pU8Val);
                        pCfgDolOfst->Pipe = DSP_IK_PIPE_VDO;

                        if ((IkAbility.VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_2) ||
                            (IkAbility.VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_3)) {
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
                            Rval = AmbaIK_QueryFrameInfo(&ImgMode, pIsoCfgCtrl[i].CfgIndex, &FrmInfo);
                            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
#else
                            dsp_wrapper_osal_typecast(&pVoid, &pIsoCfgCtrl[i].CfgAddress);
                            Rval = AmbaIK_GetIsoHdrRawOffest(pVoid, &HdrRawInfo);
                            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
#endif
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
                            for (j = 0U; j < IK_MAX_HDR_EXPOSURE_NUM; j++) {
                                pCfgDolOfst->Y[j] = FrmInfo.HdrRawInfo.YOffset[j];
                            }
#else
                            for (j = 0U; j < MAX_IK_MAX_HDR_EXPOSURE_NUM; j++) {
                                pCfgDolOfst->Y[j] = HdrRawInfo.YOffset[j];
                            }
#endif
                        } else {
                            for (j = 0U; j < DSP_IK_DOL_OFST_NUM; j++) {
                                pCfgDolOfst->Y[j] = 0U;
                            }
                        }

                        retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_IK_DRV_CFG, &DspLvIkDrvCfg);
                        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    }
                }
            }

            DspLiveviewUpdateIsocfg.NumViewZone = NumViewZone;
            Rval = dsp_wrapper_osal_memcpy(DspLiveviewUpdateIsocfg.IsoCfgCtrl, pIsoCfgCtrl, sizeof(AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s)*NumViewZone);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            /* virt2phys */
            for (i = 0U; i < NumViewZone; i++) {
                ViewZoneIkCtxId[DspLiveviewUpdateIsocfg.IsoCfgCtrl[i].ViewZoneId] = DspLiveviewUpdateIsocfg.IsoCfgCtrl[i].CtxIndex;

                Rval = dsp_wrapper_osal_virt2phys(DspLiveviewUpdateIsocfg.IsoCfgCtrl[i].CfgAddress, &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspLiveviewUpdateIsocfg.IsoCfgCtrl[i].CfgAddress = phys_addr;
            }

            retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_UPDATE_ISOCFG, &DspLiveviewUpdateIsocfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            if (pAttachedRawSeq != NULL) {
                *pAttachedRawSeq = DspLiveviewUpdateIsocfg.AttachedRawSeq;
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_LiveviewUpdateVinCfg(UINT16 VinId,
                                    UINT16 SubChNum,
                                    const AMBA_DSP_VIN_SUB_CHAN_s *pSubCh,
                                    const AMBA_DSP_LIVEVIEW_VINCFG_CTRL_s *pLvVinCfgCtrl,
                                    UINT64 *pAttachedRawSeq)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_liveview_update_vincfg_t DspLiveviewUpdateVincfg = {0};
    uint32_t i;
    ULONG phys_addr;

    /* ioctl */
    if ((pSubCh == NULL) || (pLvVinCfgCtrl == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspLiveviewUpdateVincfg.VinId = VinId;
            DspLiveviewUpdateVincfg.SubChNum = SubChNum;
            Rval = dsp_wrapper_osal_memcpy(DspLiveviewUpdateVincfg.SubCh, pSubCh, sizeof(AMBA_DSP_VIN_SUB_CHAN_s)*SubChNum);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(DspLiveviewUpdateVincfg.LvVinCfgCtrl, pLvVinCfgCtrl, sizeof(AMBA_DSP_LIVEVIEW_VINCFG_CTRL_s)*SubChNum);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            /* virt2phys */
            for (i = 0U; i < SubChNum; i++) {
                Rval = dsp_wrapper_osal_virt2phys(DspLiveviewUpdateVincfg.LvVinCfgCtrl[i].VinCompandTableAddr, &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspLiveviewUpdateVincfg.LvVinCfgCtrl[i].VinCompandTableAddr = phys_addr;

                Rval = dsp_wrapper_osal_virt2phys(DspLiveviewUpdateVincfg.LvVinCfgCtrl[i].VinDeCompandTableAddr, &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspLiveviewUpdateVincfg.LvVinCfgCtrl[i].VinDeCompandTableAddr = phys_addr;
            }

            retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_UPDATE_VINCFG, &DspLiveviewUpdateVincfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            if (pAttachedRawSeq != NULL) {
                *pAttachedRawSeq = DspLiveviewUpdateVincfg.AttachedRawSeq;
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_LiveviewUpdatePymdCfg(UINT16 NumViewZone,
                                     const UINT16 *pViewZoneId,
                                     const AMBA_DSP_LIVEVIEW_PYRAMID_s *pPyramid,
                                     const AMBA_DSP_LIVEVIEW_YUV_BUF_s *pPyramidBuf,
                                     UINT64 *pAttachedRawSeq)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    uint16_t i, j;
    int32_t dsp_fd = DSP_GET_FD();
    ULONG phys_addr;

    dsp_wrapper_osal_memset(&DspLiveviewUpdatePymdcfg, 0, sizeof(dsp_liveview_update_pymdcfg_t));

    /* ioctl */
    if ((pViewZoneId == NULL) || (pPyramid == NULL) || (pPyramidBuf == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspLiveviewUpdatePymdcfg.NumViewZone = NumViewZone;
            Rval = dsp_wrapper_osal_memcpy(&DspLiveviewUpdatePymdcfg.ViewZoneId, pViewZoneId, sizeof(UINT16)*NumViewZone);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspLiveviewUpdatePymdcfg.Pyramid, pPyramid, sizeof(AMBA_DSP_LIVEVIEW_PYRAMID_s)*NumViewZone);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            for (i = 0U; i < NumViewZone; i++) {
                DspLiveviewUpdatePymdcfg.PyramidBuf[i].AllocType = pPyramidBuf[i].AllocType;
                DspLiveviewUpdatePymdcfg.PyramidBuf[i].BufNum = pPyramidBuf[i].BufNum;
                Rval = dsp_wrapper_osal_memcpy(&DspLiveviewUpdatePymdcfg.PyramidBuf[i].YuvBuf, &pPyramidBuf[i].YuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

                /* virt2phys */
                for (j = 0U; j < pPyramidBuf[i].BufNum; j++) {
                    if (pPyramidBuf[i].pYuvBufTbl != NULL) {
                        Rval = dsp_wrapper_osal_virt2phys(pPyramidBuf[i].pYuvBufTbl[j], &phys_addr);
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                        DspLiveviewUpdatePymdcfg.PyramidBuf[i].YuvBufTbl[j] = phys_addr;
                    }
                }
            }
            retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_UPDATE_PYMDCFG, &DspLiveviewUpdatePymdcfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            if (pAttachedRawSeq != NULL) {
                *pAttachedRawSeq = DspLiveviewUpdatePymdcfg.AttachedRawSeq;
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_LiveviewConfigVinCapture(UINT16 VinId, UINT16 SubChNum, const AMBA_DSP_VIN_SUB_CHAN_CFG_s *pSubChCfg)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    uint16_t i;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_liveview_vin_cap_cfg_t DspLiveviewVinCapCfg = {0};
    uint16_t actual_vin_id = (VinId & 0x7FFFU);
    AMBA_VIN_WINDOW_s VinWin = {0};
    AMBA_VIN_INFO_s VinInfo = {0};
    ULONG DrvCfgAddr;
    uint32_t DrvCfgSize, DataSize;
    void *pVoid;
    uint8_t *pU8Val;
    dsp_liveview_vin_drv_cfg_t DspLiveviewVinDrvCfg = {0};
    dsp_vin_drv_cfg_cfa_t *pCfgCfa = NULL;

    /* ioctl */
    if (pSubChCfg == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            /* Set VinCap and get VinCfg */
            if ((VinId & 0x8000U) == 0U) {
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
                if (pSubChCfg[0U].Option != AMBA_DSP_VIN_CAP_OPT_EMBD)
#endif
                {
                    VinWin.Width = pSubChCfg[0U].CaptureWindow.Width;
                    VinWin.Height = pSubChCfg[0U].CaptureWindow.Height;
                    VinWin.OffsetX = pSubChCfg[0U].CaptureWindow.OffsetX;
                    VinWin.OffsetY = pSubChCfg[0U].CaptureWindow.OffsetY;
                    Rval = AmbaVIN_CaptureConfig(actual_vin_id, &VinWin);
                    if (Rval != DSP_ERR_NONE) {
                        dsp_wrapper_osal_printU5("[ERR] AmbaDSP_LiveviewConfigVinCapture() : VIN_CaptureConfig fail ret = 0x%X", Rval, 0U, 0U, 0U, 0U);
                    }

                    if (Rval == DSP_ERR_NONE) {
                        Rval = AmbaVIN_GetMainCfgBufInfo(actual_vin_id, &DrvCfgAddr, &DrvCfgSize);
                        if (Rval != DSP_ERR_NONE) {
                            dsp_wrapper_osal_printU5("[ERR] AmbaDSP_LiveviewConfigVinCapture() : VIN_GetMainCfgBufInfo fail 0x%X", Rval, 0U, 0U, 0U, 0U);
                        } else {
                            DataSize = (uint32_t)sizeof(DspLiveviewVinDrvCfg.VinDrvCfg.Data);
                            if (DrvCfgSize > DataSize) {
                                dsp_wrapper_osal_printU5("[ERR] AmbaDSP_LiveviewConfigVinCapture() : VIN_GetMainCfgBufInfo fail Data size exceed %d %d", DrvCfgSize, DataSize, 0U, 0U, 0U);
                                Rval = DSP_ERR_0001;
                            }
                        }
                    }

                    if (Rval == DSP_ERR_NONE) {
                        DspLiveviewVinDrvCfg.VinId = actual_vin_id;
                        DspLiveviewVinDrvCfg.Type = DSP_VIN_DRV_CFG_TYPE_DATA;
                        dsp_wrapper_osal_typecast(&pVoid, &DrvCfgAddr);
                        Rval = dsp_wrapper_osal_memcpy(DspLiveviewVinDrvCfg.VinDrvCfg.Data, pVoid, DrvCfgSize);
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                        if (0U == IS_SUCCESSOR_STATE()) {
                            retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_VIN_DRV_CFG, &DspLiveviewVinDrvCfg);
                            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
                        }
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    }

                    if (Rval == DSP_ERR_NONE) {
                        Rval = AmbaVIN_GetInfo(actual_vin_id, &VinInfo);
                        if (Rval != DSP_ERR_NONE) {
                            dsp_wrapper_osal_printU5("[ERR] AmbaDSP_LiveviewConfigVinCapture() : VIN_GetInfo fail 0x%X", Rval, 0U, 0U, 0U, 0U);
                        } else {
                            Rval = dsp_wrapper_osal_memset(&DspLiveviewVinDrvCfg, 0, sizeof(dsp_liveview_vin_drv_cfg_t));
                            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                            DspLiveviewVinDrvCfg.VinId = actual_vin_id;
                            DspLiveviewVinDrvCfg.Type = DSP_VIN_DRV_CFG_TYPE_CFA;
                            pU8Val = DspLiveviewVinDrvCfg.VinDrvCfg.Data;
                            dsp_wrapper_osal_typecast(&pCfgCfa, &pU8Val);
                            Rval = dsp_wrapper_osal_memcpy(&pCfgCfa->FrameRate, &VinInfo.FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
                            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                            pCfgCfa->ColorSpace = VinInfo.ColorSpace;
                            pCfgCfa->BayerPattern = VinInfo.BayerPattern;
                            pCfgCfa->YuvOrder = VinInfo.YuvOrder;
                            pCfgCfa->NumDataBits = VinInfo.NumDataBits;
                            pCfgCfa->NumSkipFrame = VinInfo.NumSkipFrame;
                            if (0U == IS_SUCCESSOR_STATE()) {
                                retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_VIN_DRV_CFG, &DspLiveviewVinDrvCfg);
                                Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
                            }
                            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                        }
                    }
                }
            }

            DspLiveviewVinCapCfg.VinId = VinId;
            DspLiveviewVinCapCfg.SubChNum = SubChNum;
            for (i = 0U; i < SubChNum; i++) {
                Rval = dsp_wrapper_osal_memcpy(&DspLiveviewVinCapCfg.SubChCfg[i].SubChan, &pSubChCfg[i].SubChan, sizeof(AMBA_DSP_VIN_SUB_CHAN_s));
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspLiveviewVinCapCfg.SubChCfg[i].Option = pSubChCfg[i].Option;
                DspLiveviewVinCapCfg.SubChCfg[i].ConCatNum = pSubChCfg[i].ConCatNum;
                DspLiveviewVinCapCfg.SubChCfg[i].IntcNum = pSubChCfg[i].IntcNum;
                Rval = dsp_wrapper_osal_memcpy(&DspLiveviewVinCapCfg.SubChCfg[i].CaptureWindow, &pSubChCfg[i].CaptureWindow, sizeof(AMBA_DSP_WINDOW_s));
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspLiveviewVinCapCfg.SubChCfg[i].TDNum = pSubChCfg[i].TDNum;
                if (pSubChCfg[i].TDNum > 0U) {
                    Rval = dsp_wrapper_osal_memcpy(DspLiveviewVinCapCfg.SubChCfg[i].TDFrmNum, pSubChCfg[i].pTDFrmNum, sizeof(UINT16)*pSubChCfg[i].TDNum);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                }
#if !defined (CONFIG_SOC_CV2FS) && !defined (CONFIG_SOC_CV22FS) && !defined (CONFIG_SOC_CV5) && !defined (CONFIG_SOC_CV52) && !defined (CONFIG_SOC_CV7)
                DspLiveviewVinCapCfg.SubChCfg[i].ProcDeciRate = pSubChCfg[i].ProcDeciRate;
#endif
            }
            if (0U == IS_SUCCESSOR_STATE()) {
                retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_VIN_CAP_CFG, &DspLiveviewVinCapCfg);
                Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_LiveviewConfigVinPost(const UINT8 Type, UINT16 VinId)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_liveview_vin_post_cfg_t DspLiveviewVinPostCfg = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspLiveviewVinPostCfg.Type = Type;
        DspLiveviewVinPostCfg.VinId = VinId;
        if (0U == IS_SUCCESSOR_STATE()) {
            retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_VIN_POST_CFG, &DspLiveviewVinPostCfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_LiveviewFeedRawData(UINT16 NumViewZone, const UINT16 *pViewZoneId, const AMBA_DSP_LIVEVIEW_EXT_RAW_s *pExtBuf)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_liveview_feed_raw_data_t DspLiveviewFeedRawData = {0};
    uint32_t i;
    ULONG phys_addr;

    /* ioctl */
    if (pViewZoneId == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspLiveviewFeedRawData.NumViewZone = NumViewZone;
            Rval = dsp_wrapper_osal_memcpy(DspLiveviewFeedRawData.ViewZoneId, pViewZoneId, sizeof(UINT16)*NumViewZone);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(DspLiveviewFeedRawData.ExtBuf, pExtBuf, sizeof(AMBA_DSP_LIVEVIEW_EXT_RAW_s)*NumViewZone);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            /* virt2phys */
            for (i = 0U; i < NumViewZone; i++) {
                Rval = dsp_wrapper_osal_virt2phys(DspLiveviewFeedRawData.ExtBuf[i].RawBuffer.BaseAddr, &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspLiveviewFeedRawData.ExtBuf[i].RawBuffer.BaseAddr = phys_addr;

                Rval = dsp_wrapper_osal_virt2phys(DspLiveviewFeedRawData.ExtBuf[i].CeBuffer.BaseAddr, &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspLiveviewFeedRawData.ExtBuf[i].CeBuffer.BaseAddr = phys_addr;
            }

            retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_FEED_RAW, &DspLiveviewFeedRawData);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_LiveviewFeedYuvData(UINT16 NumViewZone, const UINT16 *pViewZoneId, const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    uint16_t i;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_liveview_feed_yuv_data_t DspLiveviewFeedYuvData = {0};
    ULONG phys_addr;

    /* ioctl */
    if (pViewZoneId == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspLiveviewFeedYuvData.NumViewZone = NumViewZone;
            Rval = dsp_wrapper_osal_memcpy(DspLiveviewFeedYuvData.ViewZoneId, pViewZoneId, sizeof(UINT16)*NumViewZone);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            for (i = 0U; i < NumViewZone; i++) {
                Rval = dsp_wrapper_osal_memcpy(&DspLiveviewFeedYuvData.ExtBuf[i].ExtYuvBuf, &pExtYuvBuf[i].ExtYuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

                /* virt2phys */
                Rval = dsp_wrapper_osal_virt2phys(DspLiveviewFeedYuvData.ExtBuf[i].ExtYuvBuf.BaseAddrY, &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspLiveviewFeedYuvData.ExtBuf[i].ExtYuvBuf.BaseAddrY = phys_addr;

                Rval = dsp_wrapper_osal_virt2phys(DspLiveviewFeedYuvData.ExtBuf[i].ExtYuvBuf.BaseAddrUV, &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspLiveviewFeedYuvData.ExtBuf[i].ExtYuvBuf.BaseAddrUV = phys_addr;

                if (pExtYuvBuf[i].pExtME1Buf != NULL) {
                    Rval = dsp_wrapper_osal_memcpy(&DspLiveviewFeedYuvData.ExtBuf[i].ExtME1Buf, pExtYuvBuf[i].pExtME1Buf, sizeof(AMBA_DSP_BUF_s));
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

                    /* virt2phys */
                    Rval = dsp_wrapper_osal_virt2phys(DspLiveviewFeedYuvData.ExtBuf[i].ExtME1Buf.BaseAddr, &phys_addr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    DspLiveviewFeedYuvData.ExtBuf[i].ExtME1Buf.BaseAddr = phys_addr;
                } else {
                    Rval = dsp_wrapper_osal_memset(&DspLiveviewFeedYuvData.ExtBuf[i].ExtME1Buf, 0, sizeof(AMBA_DSP_BUF_s));
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                }
                if (pExtYuvBuf[i].pExtME0Buf != NULL) {
                    Rval = dsp_wrapper_osal_memcpy(&DspLiveviewFeedYuvData.ExtBuf[i].ExtME0Buf, pExtYuvBuf[i].pExtME0Buf, sizeof(AMBA_DSP_BUF_s));
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

                    /* virt2phys */
                    Rval = dsp_wrapper_osal_virt2phys(DspLiveviewFeedYuvData.ExtBuf[i].ExtME0Buf.BaseAddr, &phys_addr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    DspLiveviewFeedYuvData.ExtBuf[i].ExtME0Buf.BaseAddr = phys_addr;
                } else {
                    Rval = dsp_wrapper_osal_memset(&DspLiveviewFeedYuvData.ExtBuf[i].ExtME0Buf, 0, sizeof(AMBA_DSP_BUF_s));
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                }
                DspLiveviewFeedYuvData.ExtBuf[i].IsLast = pExtYuvBuf[i].IsLast;
            }
            retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_FEED_YUV, &DspLiveviewFeedYuvData);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_LiveviewYuvStreamSync(UINT16 YuvStrmIdx, const AMBA_DSP_LIVEVIEW_SYNC_CTRL_s *pYuvStrmSyncCtrl, UINT32 *pSyncJobId, UINT64 *pAttachedRawSeq)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_liveview_yuvstrm_sync_t DspLiveviewYuvStrmSync = {0};

    /* ioctl */
    if ((pYuvStrmSyncCtrl == NULL) || (pSyncJobId == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspLiveviewYuvStrmSync.YuvStrmIdx = YuvStrmIdx;
            Rval = dsp_wrapper_osal_memcpy(&DspLiveviewYuvStrmSync.YuvStrmSyncCtrl, pYuvStrmSyncCtrl, sizeof(AMBA_DSP_LIVEVIEW_SYNC_CTRL_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_YUVSTRM_SYNC, &DspLiveviewYuvStrmSync);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);

            if (DspLiveviewYuvStrmSync.YuvStrmSyncCtrl.Opt == AMBA_DSP_YUVSTRM_SYNC_START) {
                *pSyncJobId = DspLiveviewYuvStrmSync.SyncJobId;
            }
            if (pAttachedRawSeq != NULL) {
                *pAttachedRawSeq = DspLiveviewYuvStrmSync.AttachedRawSeq;
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_LiveviewUpdateGeoCfg(UINT16 ViewZoneId, const AMBA_DSP_LIVEVIEW_GEOCFG_CTRL_s *pGeoCfgCtrl, UINT64 *pAttachedRawSeq)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_liveview_update_geocfg_t DspLiveviewUpdateGeocfg = {0};
    ULONG phys_addr;

    /* ioctl */
    if (pGeoCfgCtrl == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspLiveviewUpdateGeocfg.ViewZoneId = ViewZoneId;
            Rval = dsp_wrapper_osal_memcpy(&DspLiveviewUpdateGeocfg.GeoCfgCtrl, pGeoCfgCtrl, sizeof(AMBA_DSP_LIVEVIEW_GEOCFG_CTRL_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            /* virt2phys */
            Rval = dsp_wrapper_osal_virt2phys(DspLiveviewUpdateGeocfg.GeoCfgCtrl.GeoCfgAddr, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspLiveviewUpdateGeocfg.GeoCfgCtrl.GeoCfgAddr = phys_addr;

            retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_UPDATE_GEOCFG, &DspLiveviewUpdateGeocfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            if (pAttachedRawSeq != NULL) {
                *pAttachedRawSeq = DspLiveviewUpdateGeocfg.AttachedRawSeq;
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_LiveviewUpdateVZSource(UINT16 NumViewZone, const AMBA_DSP_LIVEVIEW_VZ_SRC_CFG_s *pVzSrcCfg, UINT64 *pAttachedRawSeq)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_liveview_update_vz_src_t DspLiveviewUpdateVzSrc = {0};

    /* ioctl */
    if (pVzSrcCfg == NULL) {
        Rval = DSP_ERR_0000;
    } else if (NumViewZone >= AMBA_DSP_MAX_VIEWZONE_NUM) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspLiveviewUpdateVzSrc.NumViewZone = NumViewZone;
            Rval = dsp_wrapper_osal_memcpy(&DspLiveviewUpdateVzSrc.VzSrcCfg, pVzSrcCfg, sizeof(AMBA_DSP_LIVEVIEW_VZ_SRC_CFG_s)*NumViewZone);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_UPDATE_VZ_SRC, &DspLiveviewUpdateVzSrc);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            if (pAttachedRawSeq != NULL) {
                *pAttachedRawSeq = DspLiveviewUpdateVzSrc.AttachedRawSeq;
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_LiveviewSideBandUpdate(UINT16 ViewZoneId, UINT8 NumBand, const ULONG *pSidebandBufAddr)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_liveview_update_sideband_t DspLiveviewUpdateSideband = {0};
    ULONG phys_addr;
    uint16_t i;

    /* ioctl */
    if (pSidebandBufAddr == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspLiveviewUpdateSideband.ViewZoneId = ViewZoneId;
            DspLiveviewUpdateSideband.NumBand = NumBand;
            for (i = 0U; i<NumBand; i++) {
                /* virt2phys */
                Rval = dsp_wrapper_osal_virt2phys(pSidebandBufAddr[i], &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspLiveviewUpdateSideband.SidebandBufAddr[i] = phys_addr;
            }
            retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_UPDATE_SIDEBAND, &DspLiveviewUpdateSideband);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_LiveviewSliceCfg(UINT16 ViewZoneId, const AMBA_DSP_LIVEVIEW_SLICE_CFG_s *pLvSliceCfg)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_liveview_slice_cfg_t DspLiveviewSliceCfg = {0};

    /* ioctl */
    if (pLvSliceCfg == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspLiveviewSliceCfg.ViewZoneId = ViewZoneId;
            Rval = dsp_wrapper_osal_memcpy(&DspLiveviewSliceCfg.LvSliceCfg, pLvSliceCfg, sizeof(AMBA_DSP_LIVEVIEW_SLICE_CFG_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            if (0U == IS_SUCCESSOR_STATE()) {
                retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_SLICE_CFG, &DspLiveviewSliceCfg);
                Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_LiveviewSliceCfgCalc(UINT16 ViewZoneId, AMBA_DSP_LIVEVIEW_SLICE_CFG_s *pLvSliceCfg)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_liveview_slice_cfg_t DspLiveviewSliceCfg = {0};

    /* ioctl */
    if (pLvSliceCfg == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspLiveviewSliceCfg.ViewZoneId = ViewZoneId;
            retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_SLICE_CFG_CALC, &DspLiveviewSliceCfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            if (Rval == DSP_ERR_NONE) {
                Rval = dsp_wrapper_osal_memcpy(pLvSliceCfg, &DspLiveviewSliceCfg.LvSliceCfg, sizeof(AMBA_DSP_LIVEVIEW_SLICE_CFG_s));
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_LiveviewVZPostponeCfg(UINT16 ViewZoneId, const AMBA_DSP_LV_VZ_POSTPONE_CFG_s *pVzPostPoneCfg)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_liveview_vz_postpone_cfg_t DspLiveviewVzPostponeCfg = {0};

    /* ioctl */
    if (pVzPostPoneCfg == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspLiveviewVzPostponeCfg.ViewZoneId = ViewZoneId;
            Rval = dsp_wrapper_osal_memcpy(&DspLiveviewVzPostponeCfg.VzPostPoneCfg, pVzPostPoneCfg, sizeof(AMBA_DSP_LV_VZ_POSTPONE_CFG_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            if (0U == IS_SUCCESSOR_STATE()) {
                retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_VZ_POSTPONE_CFG, &DspLiveviewVzPostponeCfg);
                Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_LiveviewUpdateVinState(UINT16 NumVin, const AMBA_DSP_LV_VIN_STATE_s *pVinState)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_liveview_update_vin_state_t DspLiveviewUpdateVinState = {0};

    /* ioctl */
    if (pVinState == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspLiveviewUpdateVinState.NumVin = NumVin;
            Rval = dsp_wrapper_osal_memcpy(&DspLiveviewUpdateVinState.VinState, pVinState, sizeof(AMBA_DSP_LV_VIN_STATE_s)*NumVin);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_UPDATE_VIN_STATE, &DspLiveviewUpdateVinState);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_LiveviewSlowShutterCtrl(UINT16 NumViewZone, const UINT16* pViewZoneId, const UINT32* pRatio)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_liveview_slow_shutter_ctrl_t DspLiveviewSlowShutterCtrl = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspLiveviewSlowShutterCtrl.NumViewZone = NumViewZone;
        Rval = dsp_wrapper_osal_memcpy(&DspLiveviewSlowShutterCtrl.ViewZoneId, pViewZoneId, sizeof(UINT16)*NumViewZone);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        Rval = dsp_wrapper_osal_memcpy(&DspLiveviewSlowShutterCtrl.Ratio, pRatio, sizeof(UINT32)*NumViewZone);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
        retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_SLOW_SHUTTER_CTRL, &DspLiveviewSlowShutterCtrl);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
UINT32 AmbaDSP_LiveviewDropRepeatCtrl(UINT16 NumViewZone, const UINT16* pViewZoneId, const AMBA_DSP_LIVEVIEW_DROP_REPEAT_s* pDropRptCfg)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    uint32_t i;
    dsp_liveview_drop_repeat_ctrl_t DspLiveviewDropRepeatCtrl = {0};

    if ((pViewZoneId == NULL) || (pDropRptCfg == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        /* ioctl */
        if (dsp_fd > 0) {
            DspLiveviewDropRepeatCtrl.NumViewZone = NumViewZone;
            Rval = dsp_wrapper_osal_memcpy(&DspLiveviewDropRepeatCtrl.ViewZoneId, pViewZoneId, sizeof(UINT16)*NumViewZone);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            for (i = 0U; i < NumViewZone; i++) {
                DspLiveviewDropRepeatCtrl.Method[i] = pDropRptCfg[i].Method;
                DspLiveviewDropRepeatCtrl.Option[i] = pDropRptCfg[i].Option;
            }
            retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_DROP_REPEAT_CTRL, &DspLiveviewDropRepeatCtrl);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}
#endif

UINT32 AmbaDSP_LiveviewParseVpMsg(ULONG VpMsgAddr);
UINT32 AmbaDSP_LiveviewParseVpMsg(ULONG VpMsgAddr)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();

    /* ioctl */
    if (dsp_fd > 0) {
        ULONG phys_uladdr;

        /* virt2phys */
        Rval = dsp_wrapper_osal_virt2phys(VpMsgAddr, &phys_uladdr);
        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

        retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_PARSE_VP_MSG, &phys_uladdr);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

UINT32 AmbaDSP_LiveviewGetIDspCfg(UINT16 ViewZoneId, ULONG *CfgAddr);
UINT32 AmbaDSP_LiveviewGetIDspCfg(UINT16 ViewZoneId, ULONG *CfgAddr)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_liveview_idsp_cfg_t DspLiveviewIdspCfg = {0};
    UINT16 i, j;
    ULONG ULAddr;
    IDSP_INFO_s *pIdspInfo = NULL;

    /* ioctl */
    if (CfgAddr == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspLiveviewIdspCfg.ViewZoneId = ViewZoneId;
            retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_GET_IDSPCFG, &DspLiveviewIdspCfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            if (Rval == DSP_ERR_NONE) {
                Rval = dsp_wrapper_osal_phys2virt((UINT32)DspLiveviewIdspCfg.IdspCfgAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                *CfgAddr = ULAddr;

                dsp_wrapper_osal_typecast(&pIdspInfo, &ULAddr);
                /* P2V */
                /* C2Y */
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->C2Y.Sec2Cfg.CfgAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->C2Y.Sec2Cfg.CfgAddr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->C2Y.Sec4Cfg.CfgAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->C2Y.Sec4Cfg.CfgAddr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->C2Y.Sec2R2y.LumaAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->C2Y.Sec2R2y.LumaAddr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->C2Y.Sec2R2y.ChromaAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->C2Y.Sec2R2y.ChromaAddr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->C2Y.Sec2Me1.Addr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->C2Y.Sec2Me1.Addr = ULAddr;

                /* WarpMctf */
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->vWarpMctf.Sec3Cfg.CfgAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->vWarpMctf.Sec3Cfg.CfgAddr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->vWarpMctf.MctfCfg.CfgAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->vWarpMctf.MctfCfg.CfgAddr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->vWarpMctf.MctsCfg.CfgAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->vWarpMctf.MctsCfg.CfgAddr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->vWarpMctf.Sec3Me1.Addr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->vWarpMctf.Sec3Me1.Addr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->vWarpMctf.Mctf.LumaAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->vWarpMctf.Mctf.LumaAddr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->vWarpMctf.Mctf.ChromaAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->vWarpMctf.Mctf.ChromaAddr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->vWarpMctf.Mcts.LumaAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->vWarpMctf.Mcts.LumaAddr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->vWarpMctf.Mcts.ChromaAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->vWarpMctf.Mcts.ChromaAddr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->vWarpMctf.Mctf2bit.Addr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->vWarpMctf.Mctf2bit.Addr = ULAddr;

                /* Me1 */
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->Me1Smooth.Sec2Cfg.CfgAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->Me1Smooth.Sec2Cfg.CfgAddr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->Me1Smooth.Sec6Cfg.CfgAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->Me1Smooth.Sec6Cfg.CfgAddr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->Me1Smooth.Sec5Cfg.CfgAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->Me1Smooth.Sec5Cfg.CfgAddr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->Me1Smooth.vWarpMe1.Addr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->Me1Smooth.vWarpMe1.Addr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->Me1Smooth.Me1Dn3.Addr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->Me1Smooth.Me1Dn3.Addr = ULAddr;

                /* Md1 */
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->Md.Sec2Cfg[0U].CfgAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->Md.Sec2Cfg[0U].CfgAddr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->Md.Sec2Cfg[1U].CfgAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->Md.Sec2Cfg[1U].CfgAddr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->Md.Sec2Cfg[2U].CfgAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->Md.Sec2Cfg[2U].CfgAddr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->Md.Sec6Cfg.CfgAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->Md.Sec6Cfg.CfgAddr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->Md.Sec5Cfg.CfgAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->Md.Sec5Cfg.CfgAddr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->Md.MctfCfg.CfgAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->Md.MctfCfg.CfgAddr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->Md.MdMbB.Addr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->Md.MdMbB.Addr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->Md.MdMbC.Addr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->Md.MdMbC.Addr = ULAddr;

                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->Md.MoAsf.Addr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->Md.MoAsf.Addr = ULAddr;
                /* CfaAaa */
                for (i = 0U; i < IDSP_MAX_AAA_SLICE_NUM; i++) {
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->CfaAAA.Addr[i], &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->CfaAAA.Addr[i] = ULAddr;
                }
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->CfaAAA.DstAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->CfaAAA.DstAddr = ULAddr;

                /* PgAAA */
                for (i = 0U; i < IDSP_MAX_AAA_SLICE_NUM; i++) {
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->PgAAA.Addr[i], &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->PgAAA.Addr[i] = ULAddr;
                }
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->PgAAA.DstAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->PgAAA.DstAddr = ULAddr;

                /* HistAAA */
                for (j = 0U; j < DSP_VIN_HIST_NUM; j++) {
                    for (i = 0U; i < IDSP_MAX_AAA_SLICE_NUM; i++) {
                        Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HistAAA[j].Addr[i], &ULAddr);
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                        pIdspInfo->HistAAA[j].Addr[i] = ULAddr;
                    }
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HistAAA[j].DstAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HistAAA[j].DstAddr = ULAddr;
                }

                /* VinCfg */
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->VinCfg.VinSecCfg.CfgAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->VinCfg.VinSecCfg.CfgAddr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->VinCfg.CompandTblAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->VinCfg.CompandTblAddr = ULAddr;
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->VinCfg.DeCompandTblAddr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->VinCfg.DeCompandTblAddr = ULAddr;

                /* Sec2TileCfg */
                for (j = 0U; j < DSP_C2Y_TILE_Y_NUM; j++) {
                    for (i = 0U; i < DSP_C2Y_TILE_X_NUM; i++) {
                        Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->Sec2TileCfg[i][j].CfgAddr, &ULAddr);
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                        pIdspInfo->Sec2TileCfg[i][j].CfgAddr = ULAddr;
                    }
                }

                /* HISO */
                for (i = 0U; i < DSP_HISO_STEP_NUM; i++) {
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HISO[i].Sec2Cfg.CfgAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[i].Sec2Cfg.CfgAddr = ULAddr;
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HISO[i].Sec4Cfg.CfgAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[i].Sec4Cfg.CfgAddr = ULAddr;
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HISO[i].Sec5Cfg.CfgAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[i].Sec5Cfg.CfgAddr = ULAddr;
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HISO[i].Sec6Cfg.CfgAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[i].Sec6Cfg.CfgAddr = ULAddr;
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HISO[i].Sec7Cfg.CfgAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[i].Sec7Cfg.CfgAddr = ULAddr;
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HISO[i].Sec9Cfg.CfgAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[i].Sec9Cfg.CfgAddr = ULAddr;
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HISO[i].MctfCfg.CfgAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[i].MctfCfg.CfgAddr = ULAddr;
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HISO[i].MctsCfg.CfgAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[i].MctsCfg.CfgAddr = ULAddr;

                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HISO[i].Sec2.LumaAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[i].Sec2.LumaAddr = ULAddr;
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HISO[i].Sec2.ChromaAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[i].Sec2.ChromaAddr = ULAddr;
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HISO[i].Sec5.LumaAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[i].Sec5.LumaAddr = ULAddr;
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HISO[i].Sec5.ChromaAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[i].Sec5.ChromaAddr = ULAddr;
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HISO[i].Sec6.LumaAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[i].Sec6.LumaAddr = ULAddr;
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HISO[i].Sec6.ChromaAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[i].Sec6.ChromaAddr = ULAddr;
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HISO[i].Sec7.LumaAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[i].Sec7.LumaAddr = ULAddr;
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HISO[i].Sec7.ChromaAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[i].Sec7.ChromaAddr = ULAddr;
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HISO[i].Sec9.LumaAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[i].Sec9.LumaAddr = ULAddr;
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HISO[i].Sec9.ChromaAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[i].Sec9.ChromaAddr = ULAddr;
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HISO[i].Mctf.LumaAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[i].Mctf.LumaAddr = ULAddr;
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HISO[i].Mctf.ChromaAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[i].Mctf.ChromaAddr = ULAddr;
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HISO[i].Mcts.LumaAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[i].Mcts.LumaAddr = ULAddr;
                    Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->HISO[i].Mcts.ChromaAddr, &ULAddr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[i].Mcts.ChromaAddr = ULAddr;
                }

                /* Sec3TileCfg */
                for (j = 0U; j < DSP_C2Y_TILE_Y_NUM; j++) {
                    for (i = 0U; i < DSP_C2Y_TILE_X_NUM; i++) {
                        Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->Sec3TileCfg[i][j].CfgAddr, &ULAddr);
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                        pIdspInfo->Sec3TileCfg[i][j].CfgAddr = ULAddr;
                    }
                }

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
                /* Sec4TileCfg */
                for (j = 0U; j < DSP_C2Y_TILE_Y_NUM; j++) {
                    for (i = 0U; i < DSP_C2Y_TILE_X_NUM; i++) {
                        Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->Sec4TileCfg[i][j].CfgAddr, &ULAddr);
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                        pIdspInfo->Sec4TileCfg[i][j].CfgAddr = ULAddr;
                    }
                }

                /* MctfTileCfg */
                for (j = 0U; j < DSP_C2Y_TILE_Y_NUM; j++) {
                    for (i = 0U; i < DSP_C2Y_TILE_X_NUM; i++) {
                        Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->MctfTileCfg[i][j].CfgAddr, &ULAddr);
                        dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                        pIdspInfo->MctfTileCfg[i][j].CfgAddr = ULAddr;
                    }
                }

                /* MainY12 */
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->MainY12.Addr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->MainY12.Addr = ULAddr;

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
                /* MainIr */
                Rval = dsp_wrapper_osal_phys2virt((UINT32)pIdspInfo->MainIr.Addr, &ULAddr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                pIdspInfo->MainIr.Addr = ULAddr;
#endif

                /* Mem block */
                {
                    UINT32 IsCached = 0U;
                    Rval = dsp_wrapper_osal_get_buf_pool_Info(DSP_WRAP_POOL_WORK,
                                                             &pIdspInfo->MemBlk[0U/*DspWork*/].VirtBase,
                                                             &pIdspInfo->MemBlk[0U/*DspWork*/].PhysBase,
                                                             &pIdspInfo->MemBlk[0U/*DspWork*/].Size,
                                                             &IsCached);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    Rval = dsp_wrapper_osal_get_buf_pool_Info(DSP_WRAP_POOL_DATA,
                                                             &pIdspInfo->MemBlk[1U/*DspData*/].VirtBase,
                                                             &pIdspInfo->MemBlk[1U/*DspData*/].PhysBase,
                                                             &pIdspInfo->MemBlk[1U/*DspData*/].Size,
                                                             &IsCached);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                }
#endif
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VideoEncConfig(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *pStreamConfig)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_video_enc_cfg_t DspVideoEncCfg = {0};
    uint32_t i;
    ULONG phys_addr;

    /* ioctl */
    if ((pStreamIdx == NULL) || (pStreamConfig == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspVideoEncCfg.NumStream = NumStream;
            Rval = dsp_wrapper_osal_memcpy(DspVideoEncCfg.StreamIdx, pStreamIdx, sizeof(UINT16)*NumStream);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(DspVideoEncCfg.StreamConfig, pStreamConfig, sizeof(AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s)*NumStream);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            /* virt2phys */
            for (i = 0; i < NumStream; i++) {
                Rval = dsp_wrapper_osal_virt2phys(DspVideoEncCfg.StreamConfig[i].EncConfig.BitsBufAddr, &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspVideoEncCfg.StreamConfig[i].EncConfig.BitsBufAddr = phys_addr;

                Rval = dsp_wrapper_osal_virt2phys(DspVideoEncCfg.StreamConfig[i].EncConfig.QuantMatrixAddr, &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspVideoEncCfg.StreamConfig[i].EncConfig.QuantMatrixAddr = phys_addr;

                Rval = dsp_wrapper_osal_virt2phys(DspVideoEncCfg.StreamConfig[i].EncConfig.JpgBitRateCtrl.RateCurvAddr, &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspVideoEncCfg.StreamConfig[i].EncConfig.JpgBitRateCtrl.RateCurvAddr = phys_addr;
            }

            retcode = ioctl(dsp_fd, AMBADSP_VIDEO_ENC_CFG, &DspVideoEncCfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VideoEncStart(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_START_CONFIG_s *pStartConfig, UINT64 *pAttachedRawSeq)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_video_enc_start_t DspVideoEncStart = {0};

    /* ioctl */
    if ((pStreamIdx == NULL) || (pStartConfig == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspVideoEncStart.NumStream = NumStream;
            Rval = dsp_wrapper_osal_memcpy(DspVideoEncStart.StreamIdx, pStreamIdx, sizeof(UINT16)*NumStream);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(DspVideoEncStart.StartConfig, pStartConfig, sizeof(AMBA_DSP_VIDEO_ENC_START_CONFIG_s)*NumStream);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            retcode = ioctl(dsp_fd, AMBADSP_VIDEO_ENC_START, &DspVideoEncStart);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            if (pAttachedRawSeq != NULL) {
                *pAttachedRawSeq = DspVideoEncStart.AttachedRawSeq;
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VideoEncStop(UINT16 NumStream, const UINT16 *pStreamIdx, const UINT8 *pStopOption, UINT64 *pAttachedRawSeq)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_video_enc_stop_t DspVideoEncStop = {0};

    /* ioctl */
    if ((pStreamIdx == NULL) || (pStopOption == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspVideoEncStop.NumStream = NumStream;
            Rval = dsp_wrapper_osal_memcpy(DspVideoEncStop.StreamIdx, pStreamIdx, sizeof(UINT16)*NumStream);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(DspVideoEncStop.StopOption, pStopOption, sizeof(UINT8)*NumStream);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            retcode = ioctl(dsp_fd, AMBADSP_VIDEO_ENC_STOP, &DspVideoEncStop);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            if (pAttachedRawSeq != NULL) {
                *pAttachedRawSeq = DspVideoEncStop.AttachedRawSeq;
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VideoEncControlFrameRate(UINT16 NumStream, const UINT16 *pStreamIdx, const UINT32 *pDivisor, UINT64 *pAttachedRawSeq)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_video_enc_frmrate_ctrl_t DspVideoEncFrmRateCtrl = {0};

    /* ioctl */
    if ((pStreamIdx == NULL) || (pDivisor == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspVideoEncFrmRateCtrl.NumStream = NumStream;
            Rval = dsp_wrapper_osal_memcpy(DspVideoEncFrmRateCtrl.StreamIdx, pStreamIdx, sizeof(UINT16)*NumStream);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(DspVideoEncFrmRateCtrl.Divisor, pDivisor, sizeof(UINT32)*NumStream);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            retcode = ioctl(dsp_fd, AMBADSP_VIDEO_ENC_FRMRATE_CTRL, &DspVideoEncFrmRateCtrl);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            if (pAttachedRawSeq != NULL) {
                *pAttachedRawSeq = DspVideoEncFrmRateCtrl.AttachedRawSeq;
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VideoEncControlRepeatDrop(UINT16 NumStream, const UINT16* pStreamIdx, const AMBA_DSP_VIDEO_REPEAT_DROP_CONFIG_s *pRepeatDropCfg, const UINT64* pAttachedRawSeq)
{
(void)NumStream;
(void)pStreamIdx;
(void)pRepeatDropCfg;
(void)pAttachedRawSeq;
    return DSP_ERR_0008;
}

UINT32 AmbaDSP_VideoEncControlSlowShutter(UINT16 NumVin, const UINT16 *pVinIdx, const UINT32 *pUpSamplingRate, const UINT64 *pAttachedRawSeq)
{
(void)NumVin;
(void)pVinIdx;
(void)pUpSamplingRate;
(void)pAttachedRawSeq;

    dsp_wrapper_osal_printU5("AmbaDSP_VideoEncControlSlowShutter is not supported anymore, please use AmbaDSP_LiveviewSlowShutterCtrl", 0U, 0U, 0U, 0U, 0U);

    return DSP_ERR_0008;
}

UINT32 AmbaDSP_VideoEncControlBlend(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_BLEND_CONFIG_s *pBlendCfg)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_video_enc_blend_ctrl_t DspVideoEncBlendCtrl = {0};
    uint32_t i;
    ULONG phys_addr;

    /* ioctl */
    if (pStreamIdx == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspVideoEncBlendCtrl.NumStream = NumStream;
            Rval = dsp_wrapper_osal_memcpy(DspVideoEncBlendCtrl.StreamIdx, pStreamIdx, sizeof(UINT16)*NumStream);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(DspVideoEncBlendCtrl.BlendCfg, pBlendCfg, sizeof(AMBA_DSP_VIDEO_BLEND_CONFIG_s)*NumStream);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            /* virt2phys */
            for (i=0; i<NumStream; i++) {
                Rval = dsp_wrapper_osal_virt2phys(DspVideoEncBlendCtrl.BlendCfg[i].BlendYuvBuf.BaseAddrY, &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspVideoEncBlendCtrl.BlendCfg[i].BlendYuvBuf.BaseAddrY = phys_addr;

                Rval = dsp_wrapper_osal_virt2phys(DspVideoEncBlendCtrl.BlendCfg[i].BlendYuvBuf.BaseAddrUV, &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspVideoEncBlendCtrl.BlendCfg[i].BlendYuvBuf.BaseAddrUV = phys_addr;

                Rval = dsp_wrapper_osal_virt2phys(DspVideoEncBlendCtrl.BlendCfg[i].AlphaBuf.BaseAddrY, &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspVideoEncBlendCtrl.BlendCfg[i].AlphaBuf.BaseAddrY = phys_addr;

                Rval = dsp_wrapper_osal_virt2phys(DspVideoEncBlendCtrl.BlendCfg[i].AlphaBuf.BaseAddrUV, &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspVideoEncBlendCtrl.BlendCfg[i].AlphaBuf.BaseAddrUV = phys_addr;
            }
            retcode = ioctl(dsp_fd, AMBADSP_VIDEO_ENC_BLEND_CTRL, &DspVideoEncBlendCtrl);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VideoEncControlQuality(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_QUALITY_s *pQCtrl)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_video_enc_quality_ctrl_t DspVideoEncQualityCtrl = {0};

    /* ioctl */
    if (pStreamIdx == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspVideoEncQualityCtrl.NumStream = NumStream;
            Rval = dsp_wrapper_osal_memcpy(DspVideoEncQualityCtrl.StreamIdx, pStreamIdx, sizeof(UINT16)*NumStream);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(DspVideoEncQualityCtrl.QCtrl, pQCtrl, sizeof(AMBA_DSP_VIDEO_ENC_QUALITY_s)*NumStream);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            retcode = ioctl(dsp_fd, AMBADSP_VIDEO_ENC_QUALITY_CTRL, &DspVideoEncQualityCtrl);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VideoEncExecIntervalCap(UINT16 NumStream, const UINT16 *pStreamIdx)
{
(void)NumStream;
(void)pStreamIdx;
    return DSP_ERR_0008;
}

UINT32 AmbaDSP_VideoEncFeedYuvData(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_video_enc_feed_yuv_data_t DspVideoEncFeedYuv = {0};
    uint32_t i;
    ULONG phys_addr;

    /* ioctl */
    if ((pStreamIdx == NULL) || (pExtYuvBuf == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspVideoEncFeedYuv.NumStream = NumStream;
            Rval = dsp_wrapper_osal_memcpy(DspVideoEncFeedYuv.StreamIdx, pStreamIdx, sizeof(UINT16)*NumStream);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            for (i = 0U; i < NumStream; i++) {
                Rval = dsp_wrapper_osal_memcpy(&DspVideoEncFeedYuv.ExtBuf[i].ExtYuvBuf, &pExtYuvBuf[i].ExtYuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

                /* virt2phys */
                Rval = dsp_wrapper_osal_virt2phys(DspVideoEncFeedYuv.ExtBuf[i].ExtYuvBuf.BaseAddrY, &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspVideoEncFeedYuv.ExtBuf[i].ExtYuvBuf.BaseAddrY = phys_addr;

                Rval = dsp_wrapper_osal_virt2phys(DspVideoEncFeedYuv.ExtBuf[i].ExtYuvBuf.BaseAddrUV, &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspVideoEncFeedYuv.ExtBuf[i].ExtYuvBuf.BaseAddrUV = phys_addr;

                if (pExtYuvBuf[i].pExtME1Buf != NULL) {
                    Rval = dsp_wrapper_osal_memcpy(&DspVideoEncFeedYuv.ExtBuf[i].ExtME1Buf, pExtYuvBuf[i].pExtME1Buf, sizeof(AMBA_DSP_BUF_s));
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

                    /* virt2phys */
                    Rval = dsp_wrapper_osal_virt2phys(DspVideoEncFeedYuv.ExtBuf[i].ExtME1Buf.BaseAddr, &phys_addr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    DspVideoEncFeedYuv.ExtBuf[i].ExtME1Buf.BaseAddr = phys_addr;
                }
                if (pExtYuvBuf[i].pExtME0Buf != NULL) {
                    Rval = dsp_wrapper_osal_memcpy(&DspVideoEncFeedYuv.ExtBuf[i].ExtME0Buf, pExtYuvBuf[i].pExtME0Buf, sizeof(AMBA_DSP_BUF_s));
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

                    /* virt2phys */
                    Rval = dsp_wrapper_osal_virt2phys(DspVideoEncFeedYuv.ExtBuf[i].ExtME0Buf.BaseAddr, &phys_addr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    DspVideoEncFeedYuv.ExtBuf[i].ExtME0Buf.BaseAddr = phys_addr;
                }
                DspVideoEncFeedYuv.ExtBuf[i].IsLast = pExtYuvBuf[i].IsLast;
            }

            retcode = ioctl(dsp_fd, AMBADSP_VIDEO_ENC_FEED_YUV, &DspVideoEncFeedYuv);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VideoEncGrpConfig(const AMBA_DSP_VIDEO_ENC_GROUP_s *pGrpCfg)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    AMBA_DSP_VIDEO_ENC_GROUP_s DspVideoEncGrpCfg = {0};
    ULONG phys_addr;

    /* ioctl */
    if (pGrpCfg == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            Rval = dsp_wrapper_osal_memcpy(&DspVideoEncGrpCfg, pGrpCfg, sizeof(AMBA_DSP_VIDEO_ENC_GROUP_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            /* virt2phys */
            Rval = dsp_wrapper_osal_virt2phys(DspVideoEncGrpCfg.RcCfgAddr, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspVideoEncGrpCfg.RcCfgAddr = phys_addr;

            retcode = ioctl(dsp_fd, AMBADSP_VIDEO_ENC_GRP_CFG, &DspVideoEncGrpCfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VideoEncMvConfig(UINT16 StreamIdx, const AMBA_DSP_VIDEO_ENC_MV_CFG_s *pMvCfg)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_video_enc_mv_cfg_t DspVideoEncMvCfg = {0};
    ULONG phys_addr;

    /* ioctl */
    if (pMvCfg == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspVideoEncMvCfg.StreamIdx = StreamIdx;
            Rval = dsp_wrapper_osal_memcpy(&DspVideoEncMvCfg.MvCfg, pMvCfg, sizeof(AMBA_DSP_VIDEO_ENC_MV_CFG_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            /* virt2phys */
            Rval = dsp_wrapper_osal_virt2phys(DspVideoEncMvCfg.MvCfg.BufAddr, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspVideoEncMvCfg.MvCfg.BufAddr = phys_addr;

            retcode = ioctl(dsp_fd, AMBADSP_VIDEO_ENC_MV_CFG, &DspVideoEncMvCfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VideoEncDescFmtConfig(UINT16 StreamIdx, UINT16 CatIdx, UINT32 OptVal)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_video_enc_desc_fmt_cfg_t DspVideoEncDescFmtCfg = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspVideoEncDescFmtCfg.StreamIdx = StreamIdx;
        DspVideoEncDescFmtCfg.CatIdx = CatIdx;
        DspVideoEncDescFmtCfg.OptVal = OptVal;
        retcode = ioctl(dsp_fd, AMBADSP_VIDEO_ENC_DESC_FMT_CFG, &DspVideoEncDescFmtCfg);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

UINT32 AmbaDSP_DataCapCfg(UINT16 CapInstance, const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_stl_data_cap_cfg_t DspStlDataCapCfg = {0};
    ULONG phys_addr;
    uint32_t i;

    /* ioctl */
    if (pDataCapCfg == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspStlDataCapCfg.CapInstance = CapInstance;
            DspStlDataCapCfg.DataCapCfg.CapDataType     = pDataCapCfg->CapDataType;
            DspStlDataCapCfg.DataCapCfg.AuxDataNeeded   = pDataCapCfg->AuxDataNeeded;
            DspStlDataCapCfg.DataCapCfg.Index           = pDataCapCfg->Index;
            DspStlDataCapCfg.DataCapCfg.AllocType       = pDataCapCfg->AllocType;
            DspStlDataCapCfg.DataCapCfg.BufNum          = pDataCapCfg->BufNum;
            DspStlDataCapCfg.DataCapCfg.OverFlowCtrl    = pDataCapCfg->OverFlowCtrl;
            DspStlDataCapCfg.DataCapCfg.CmprRate        = pDataCapCfg->CmprRate;
            DspStlDataCapCfg.DataCapCfg.CmptRate        = pDataCapCfg->CmptRate;
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
            DspStlDataCapCfg.DataCapCfg.HdrExposureNum  = pDataCapCfg->HdrExposureNum;
#endif
            Rval = dsp_wrapper_osal_memcpy(&DspStlDataCapCfg.DataCapCfg.DataBuf, &pDataCapCfg->DataBuf, sizeof(AMBA_DSP_BUF_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspStlDataCapCfg.DataCapCfg.AuxDataBuf, &pDataCapCfg->AuxDataBuf, sizeof(AMBA_DSP_BUF_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            /* virt2phys */
            Rval = dsp_wrapper_osal_virt2phys(DspStlDataCapCfg.DataCapCfg.DataBuf.BaseAddr, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlDataCapCfg.DataCapCfg.DataBuf.BaseAddr = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStlDataCapCfg.DataCapCfg.AuxDataBuf.BaseAddr, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlDataCapCfg.DataCapCfg.AuxDataBuf.BaseAddr = phys_addr;

            if (pDataCapCfg->pBufTbl != NULL){
                for (i = 0; i < pDataCapCfg->BufNum; i++) {
                    Rval = dsp_wrapper_osal_virt2phys(pDataCapCfg->pBufTbl[i], &phys_addr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    DspStlDataCapCfg.DataCapCfg.BufTbl[i] = phys_addr;
                }
            }

            if (pDataCapCfg->pAuxBufTbl != NULL){
                for (i = 0; i < pDataCapCfg->BufNum; i++) {
                    Rval = dsp_wrapper_osal_virt2phys(pDataCapCfg->pAuxBufTbl[i], &phys_addr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    DspStlDataCapCfg.DataCapCfg.AuxBufTbl[i] = phys_addr;
                }
            }

            retcode = ioctl(dsp_fd, AMBADSP_STL_DATA_CAP_CFG, &DspStlDataCapCfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_UpdateCapBuffer(UINT16 CapInstance, const AMBA_DSP_DATACAP_BUF_CFG_s *pCapBuf, UINT64 *pAttachedRawSeq)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_stl_update_cap_buf_t DspStlUpdateCapBuf = {0};
    ULONG phys_addr;
    uint32_t i;

    /* ioctl */
    if (pCapBuf == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspStlUpdateCapBuf.CapInstance = CapInstance;
            DspStlUpdateCapBuf.CapBuf.BufNum        = pCapBuf->BufNum;
            DspStlUpdateCapBuf.CapBuf.AllocType     = pCapBuf->AllocType;

            /* virt2phys */
            Rval = dsp_wrapper_osal_virt2phys(pCapBuf->BufAddr, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlUpdateCapBuf.CapBuf.BufAddr = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(pCapBuf->AuxBufAddr, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlUpdateCapBuf.CapBuf.AuxBufAddr = phys_addr;

            if (pCapBuf->pBufTbl != NULL){
                for (i = 0; i < pCapBuf->BufNum; i++) {
                    Rval = dsp_wrapper_osal_virt2phys(pCapBuf->pBufTbl[i], &phys_addr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    DspStlUpdateCapBuf.CapBuf.BufTbl[i] = phys_addr;
                }
            }

            if (pCapBuf->pAuxBufTbl != NULL){
                for (i = 0; i < pCapBuf->BufNum; i++) {
                    Rval = dsp_wrapper_osal_virt2phys(pCapBuf->pAuxBufTbl[i], &phys_addr);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                    DspStlUpdateCapBuf.CapBuf.AuxBufTbl[i] = phys_addr;
                }
            }

            retcode = ioctl(dsp_fd, AMBADSP_STL_UPDATE_CAP_BUF, &DspStlUpdateCapBuf);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            if (pAttachedRawSeq != NULL) {
                *pAttachedRawSeq = DspStlUpdateCapBuf.AttachedRawSeq;
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_DataCapCtrl(UINT16 NumCapInstance, const UINT16 *pCapInstance, const AMBA_DSP_DATACAP_CTRL_s *pDataCapCtrl, UINT64 *pAttachedRawSeq)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_stl_data_cap_ctrl_t DspStlDataCapCtrl = {0};

    /* ioctl */
    if ((pCapInstance == NULL) || (pDataCapCtrl == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspStlDataCapCtrl.NumCapInstance = NumCapInstance;
            Rval = dsp_wrapper_osal_memcpy(DspStlDataCapCtrl.CapInstance, pCapInstance, sizeof(UINT16)*NumCapInstance);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(DspStlDataCapCtrl.DataCapCtrl, pDataCapCtrl, sizeof(AMBA_DSP_DATACAP_CTRL_s)*NumCapInstance);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            retcode = ioctl(dsp_fd, AMBADSP_STL_DATA_CAP_CTRL, &DspStlDataCapCtrl);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);

            if (pAttachedRawSeq != NULL) {
                *pAttachedRawSeq = DspStlDataCapCtrl.AttachedRawSeq;
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_StillYuv2Yuv(const AMBA_DSP_YUV_IMG_BUF_s *pYuvIn, const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, UINT32 Opt, UINT64 *pAttachedRawSeq)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_stl_y2y_t DspStlY2Y = {0};
    ULONG phys_addr;

    /* ioctl */
    if ((pYuvIn == NULL) || (pYuvOut == NULL) || (pIsoCfg == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            Rval = dsp_wrapper_osal_memcpy(&DspStlY2Y.YuvIn, pYuvIn, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspStlY2Y.YuvOut, pYuvOut, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspStlY2Y.IsoCfg, pIsoCfg, sizeof(AMBA_DSP_ISOCFG_CTRL_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlY2Y.Opt = Opt;

            /* virt2phys */
            Rval = dsp_wrapper_osal_virt2phys(DspStlY2Y.YuvIn.BaseAddrY, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlY2Y.YuvIn.BaseAddrY = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStlY2Y.YuvIn.BaseAddrUV, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlY2Y.YuvIn.BaseAddrUV = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStlY2Y.YuvOut.BaseAddrY, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlY2Y.YuvOut.BaseAddrY = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStlY2Y.YuvOut.BaseAddrUV, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlY2Y.YuvOut.BaseAddrUV = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStlY2Y.IsoCfg.CfgAddress, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlY2Y.IsoCfg.CfgAddress = phys_addr;

            retcode = ioctl(dsp_fd, AMBADSP_STL_Y2Y, &DspStlY2Y);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            if (pAttachedRawSeq != NULL) {
                *pAttachedRawSeq = DspStlY2Y.AttachedRawSeq;
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_StillEncodeCtrl(UINT16 StreamIdx, const AMBA_DSP_STLENC_CTRL_s *pStlEncCtrl, UINT64 *pAttachedRawSeq)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_stl_enc_ctrl_t DspStlEncCtrl = {0};
    ULONG phys_addr;

    /* ioctl */
    if (pStlEncCtrl == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspStlEncCtrl.StreamIdx = StreamIdx;
            Rval = dsp_wrapper_osal_memcpy(&DspStlEncCtrl.StlEncCtrl, pStlEncCtrl, sizeof(AMBA_DSP_STLENC_CTRL_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            /* virt2phys */
            Rval = dsp_wrapper_osal_virt2phys(DspStlEncCtrl.StlEncCtrl.QTblAddr, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlEncCtrl.StlEncCtrl.QTblAddr = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStlEncCtrl.StlEncCtrl.RateCurvAddr, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlEncCtrl.StlEncCtrl.RateCurvAddr = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStlEncCtrl.StlEncCtrl.BitsBufAddr, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlEncCtrl.StlEncCtrl.BitsBufAddr = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStlEncCtrl.StlEncCtrl.YuvBuf.BaseAddrY, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlEncCtrl.StlEncCtrl.YuvBuf.BaseAddrY = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStlEncCtrl.StlEncCtrl.YuvBuf.BaseAddrUV, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlEncCtrl.StlEncCtrl.YuvBuf.BaseAddrUV = phys_addr;

            retcode = ioctl(dsp_fd, AMBADSP_STL_ENC_CTRL, &DspStlEncCtrl);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            if (pAttachedRawSeq != NULL) {
                *pAttachedRawSeq = DspStlEncCtrl.AttachedRawSeq;
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_CalcStillYuvExtBufSize(UINT16 StreamIdx, UINT16 BufType, UINT16 *pBufPitch, UINT32 *pBufUnitSize)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_stl_yuv_extbuf_clac_t DspStlYuvExtBufCalc = {0};

    /* ioctl */
    if (pBufPitch == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspStlYuvExtBufCalc.StreamIdx = StreamIdx;
            DspStlYuvExtBufCalc.BufType = BufType;
            retcode = ioctl(dsp_fd, AMBADSP_STL_YUV_EXTBUF_CACL, &DspStlYuvExtBufCalc);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            *pBufPitch = DspStlYuvExtBufCalc.BufPitch;
            *pBufUnitSize = DspStlYuvExtBufCalc.BufUnitSize;
        }
    }

    return Rval;
}

UINT32 AmbaDSP_StillRaw2Yuv(const AMBA_DSP_RAW_BUF_s *pRawIn,
                            const AMBA_DSP_BUF_s *pAuxBufIn,
                            const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut,
                            const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                            UINT32 Opt,
                            UINT64 *pAttachedRawSeq)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_stl_r2y_t DspStlR2Y = {0};
    ULONG phys_addr;
    AMBA_IK_MODE_CFG_s ImgMode;
    AMBA_IK_ABILITY_s IkAbility = {0};
    AMBA_IK_VIN_SENSOR_INFO_s VinSensorInfo = {0};
    dsp_liveview_ik_drv_cfg_t DspLvIkDrvCfg;
    uint32_t i;
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
    AMBA_IK_QUERY_FRAME_INFO_s FrmInfo = {0};
#else
    void *pVoid;
    AMBA_IK_HDR_RAW_INFO_s HdrRawInfo = {0};
#endif
    uint8_t *pU8Val;
    dsp_ik_drv_cfg_sensor_info_t *pCfgSensorInfo;
    dsp_ik_drv_cfg_dol_offset_t *pCfgDolOfst;

    /* ioctl */
    if ((pRawIn == NULL) || (pAuxBufIn == NULL) || (pYuvOut == NULL) || (pIsoCfg == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            /* Issue IK realtive APIs */
            ImgMode.ContextId = pIsoCfg->CtxIndex;

            pU8Val = DspLvIkDrvCfg.IkDrvCfg.Data;

            Rval = dsp_wrapper_osal_memset(&DspLvIkDrvCfg, 0, sizeof(dsp_liveview_ik_drv_cfg_t));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspLvIkDrvCfg.ViewZoneId = 0U/*DONT CARE*/;
            DspLvIkDrvCfg.Type = DSP_IK_DRV_CFG_TYPE_SNSR_INFO;
            Rval = AmbaIK_GetVinSensorInfo(&ImgMode, &VinSensorInfo);
            if (Rval == DSP_ERR_NONE) {
                dsp_wrapper_osal_typecast(&pCfgSensorInfo, &pU8Val);
                pCfgSensorInfo->Pipe = DSP_IK_PIPE_STL;
                pCfgSensorInfo->SensorMode = VinSensorInfo.SensorMode;
                pCfgSensorInfo->Compression = VinSensorInfo.Compression;
                retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_IK_DRV_CFG, &DspLvIkDrvCfg);
                Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            }

            if (pIsoCfg->CfgAddress > 0U) {
                (void)AmbaIK_GetContextAbility(&ImgMode, &IkAbility);

                DspLvIkDrvCfg.Type = DSP_IK_DRV_CFG_TYPE_DOL_OFST;
                dsp_wrapper_osal_typecast(&pCfgDolOfst, &pU8Val);
                pCfgDolOfst->Pipe = DSP_IK_PIPE_STL;

                if ((IkAbility.VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_2) ||
                    (IkAbility.VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_3)) {
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
                    Rval = AmbaIK_QueryFrameInfo(&ImgMode, pIsoCfg->CfgIndex, &FrmInfo);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
#else
                    dsp_wrapper_osal_typecast(&pVoid, &pIsoCfg->CfgAddress);
                    Rval = AmbaIK_GetIsoHdrRawOffest(pVoid, &HdrRawInfo);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
#endif
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
                    for (i = 0U; i < IK_MAX_HDR_EXPOSURE_NUM; i++) {
                        pCfgDolOfst->Y[i] = FrmInfo.HdrRawInfo.YOffset[i];
                    }
#else
                    for (i = 0U; i < MAX_IK_MAX_HDR_EXPOSURE_NUM; i++) {
                        pCfgDolOfst->Y[i] = HdrRawInfo.YOffset[i];
                    }
#endif
                } else {
                    for (i = 0U; i < DSP_IK_DOL_OFST_NUM; i++) {
                        pCfgDolOfst->Y[i] = 0U;
                    }
                }
                retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_IK_DRV_CFG, &DspLvIkDrvCfg);
                Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            }

            Rval = dsp_wrapper_osal_memcpy(&DspStlR2Y.RawIn, pRawIn, sizeof(AMBA_DSP_RAW_BUF_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspStlR2Y.AuxBufIn, pAuxBufIn, sizeof(AMBA_DSP_BUF_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspStlR2Y.YuvOut, pYuvOut, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspStlR2Y.IsoCfg, pIsoCfg, sizeof(AMBA_DSP_ISOCFG_CTRL_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlR2Y.Opt = Opt;

            /* virt2phys */
            Rval = dsp_wrapper_osal_virt2phys(DspStlR2Y.RawIn.BaseAddr, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlR2Y.RawIn.BaseAddr = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStlR2Y.AuxBufIn.BaseAddr, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlR2Y.AuxBufIn.BaseAddr = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStlR2Y.YuvOut.BaseAddrY, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlR2Y.YuvOut.BaseAddrY = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStlR2Y.YuvOut.BaseAddrUV, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlR2Y.YuvOut.BaseAddrUV = phys_addr;


            Rval = dsp_wrapper_osal_virt2phys(DspStlR2Y.IsoCfg.CfgAddress, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlR2Y.IsoCfg.CfgAddress = phys_addr;

            retcode = ioctl(dsp_fd, AMBADSP_STL_R2Y, &DspStlR2Y);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            if (pAttachedRawSeq != NULL) {
                *pAttachedRawSeq = DspStlR2Y.AttachedRawSeq;
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_StillRaw2Raw(const AMBA_DSP_RAW_BUF_s *pRawIn,
                            const AMBA_DSP_BUF_s *pAuxBufIn,
                            const AMBA_DSP_RAW_BUF_s *pRawOut,
                            const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                            UINT32 Opt,
                            UINT64 *pAttachedRawSeq)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_stl_r2r_t DspStlR2R = {0};
    ULONG phys_addr;
    AMBA_IK_MODE_CFG_s ImgMode;
    AMBA_IK_ABILITY_s IkAbility = {0};
    AMBA_IK_VIN_SENSOR_INFO_s VinSensorInfo = {0};
    dsp_liveview_ik_drv_cfg_t DspLvIkDrvCfg;
    uint32_t i;
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
    AMBA_IK_QUERY_FRAME_INFO_s FrmInfo = {0};
#else
    void *pVoid;
    AMBA_IK_HDR_RAW_INFO_s HdrRawInfo = {0};
#endif
    uint8_t *pU8Val;
    dsp_ik_drv_cfg_sensor_info_t *pCfgSensorInfo;
    dsp_ik_drv_cfg_dol_offset_t *pCfgDolOfst;
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
    dsp_ik_drv_cfg_ext_raw_cmpr_info_t *pCfgExtRawCmpr;
#endif

    /* ioctl */
    if ((pRawIn == NULL) || (pAuxBufIn == NULL) || (pRawOut == NULL) || (pIsoCfg == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            ImgMode.ContextId = pIsoCfg->CtxIndex;

            pU8Val = DspLvIkDrvCfg.IkDrvCfg.Data;

            Rval = dsp_wrapper_osal_memset(&DspLvIkDrvCfg, 0, sizeof(dsp_liveview_ik_drv_cfg_t));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspLvIkDrvCfg.ViewZoneId = 0U/*DONT CARE*/;
            DspLvIkDrvCfg.Type = DSP_IK_DRV_CFG_TYPE_SNSR_INFO;
            Rval = AmbaIK_GetVinSensorInfo(&ImgMode, &VinSensorInfo);
            if (Rval == DSP_ERR_NONE) {
                dsp_wrapper_osal_typecast(&pCfgSensorInfo, &pU8Val);
                pCfgSensorInfo->Pipe = DSP_IK_PIPE_STL;
                pCfgSensorInfo->SensorMode = VinSensorInfo.SensorMode;
                pCfgSensorInfo->Compression = VinSensorInfo.Compression;
                retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_IK_DRV_CFG, &DspLvIkDrvCfg);
                Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            }

            if (pIsoCfg->CfgAddress > 0U) {
                (void)AmbaIK_GetContextAbility(&ImgMode, &IkAbility);

                dsp_wrapper_osal_typecast(&pCfgDolOfst, &pU8Val);
                DspLvIkDrvCfg.Type = DSP_IK_DRV_CFG_TYPE_DOL_OFST;
                pCfgDolOfst->Pipe = DSP_IK_PIPE_STL;

                if ((IkAbility.VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_2) ||
                    (IkAbility.VideoPipe == AMBA_IK_VIDEO_HDR_EXPO_3)) {
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
                    Rval = AmbaIK_QueryFrameInfo(&ImgMode, pIsoCfg->CfgIndex, &FrmInfo);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
#else
                    dsp_wrapper_osal_typecast(&pVoid, &pIsoCfg->CfgAddress);
                    Rval = AmbaIK_GetIsoHdrRawOffest(pVoid, &HdrRawInfo);
                    dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
#endif
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
                    for (i = 0U; i < IK_MAX_HDR_EXPOSURE_NUM; i++) {
                        pCfgDolOfst->Y[i] = FrmInfo.HdrRawInfo.YOffset[i];
                    }
#else
                    for (i = 0U; i < MAX_IK_MAX_HDR_EXPOSURE_NUM; i++) {
                        pCfgDolOfst->Y[i] = HdrRawInfo.YOffset[i];
                    }
#endif
                } else {
                    for (i = 0U; i < DSP_IK_DOL_OFST_NUM; i++) {
                        pCfgDolOfst->Y[i] = 0U;
                    }
                }
                retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_IK_DRV_CFG, &DspLvIkDrvCfg);
                Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
                Rval = dsp_wrapper_osal_memset(&DspLvIkDrvCfg, 0, sizeof(dsp_liveview_ik_drv_cfg_t));
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

                DspLvIkDrvCfg.ViewZoneId = 0U/*DONT CARE*/;
                DspLvIkDrvCfg.Type = DSP_IK_DRV_CFG_TYPE_EXT_RAW_CMPR;
                dsp_wrapper_osal_typecast(&pCfgExtRawCmpr, &pU8Val);
                pCfgExtRawCmpr->Pipe = DSP_IK_PIPE_STL;
                Rval = AmbaIK_GetExtRawOutMode(&ImgMode, &pCfgExtRawCmpr->Compression);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

                retcode = ioctl(dsp_fd, AMBADSP_LIVEVIEW_IK_DRV_CFG, &DspLvIkDrvCfg);
                Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
#endif
            }

            Rval = dsp_wrapper_osal_memcpy(&DspStlR2R.RawIn, pRawIn, sizeof(AMBA_DSP_RAW_BUF_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspStlR2R.AuxBufIn, pAuxBufIn, sizeof(AMBA_DSP_BUF_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspStlR2R.RawOut, pRawOut, sizeof(AMBA_DSP_RAW_BUF_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspStlR2R.IsoCfg, pIsoCfg, sizeof(AMBA_DSP_ISOCFG_CTRL_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlR2R.Opt = Opt;

            /* virt2phys */
            Rval = dsp_wrapper_osal_virt2phys(DspStlR2R.RawIn.BaseAddr, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlR2R.RawIn.BaseAddr = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStlR2R.AuxBufIn.BaseAddr, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlR2R.AuxBufIn.BaseAddr = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStlR2R.RawOut.BaseAddr, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlR2R.RawOut.BaseAddr = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStlR2R.IsoCfg.CfgAddress, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStlR2R.IsoCfg.CfgAddress = phys_addr;

            retcode = ioctl(dsp_fd, AMBADSP_STL_R2R, &DspStlR2R);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
            if (pAttachedRawSeq != NULL) {
                *pAttachedRawSeq = DspStlR2R.AttachedRawSeq;
            }
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VideoDecConfig(UINT16 MaxNumStream, const AMBA_DSP_VIDDEC_STREAM_CONFIG_s *pStreamConfig)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    uint32_t i;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_video_dec_cfg_t DspVideoDecCfg = {0};
    ULONG phys_addr;

    /* ioctl */
    if (pStreamConfig == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspVideoDecCfg.MaxNumStream = MaxNumStream;
            Rval = dsp_wrapper_osal_memcpy(DspVideoDecCfg.StrmCfg, pStreamConfig, sizeof(AMBA_DSP_VIDDEC_STREAM_CONFIG_s)*MaxNumStream);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            /* virt2phys */
            for (i = 0U; i < MaxNumStream; i++) {
                Rval = dsp_wrapper_osal_virt2phys(DspVideoDecCfg.StrmCfg[i].BitsBufAddr, &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspVideoDecCfg.StrmCfg[i].BitsBufAddr = phys_addr;
            }

            retcode = ioctl(dsp_fd, AMBADSP_VIDEO_DEC_CFG, &DspVideoDecCfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VideoDecStart(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_video_dec_start_t DspVideoDecStart = {0};

    /* ioctl */
    if ((pStreamIdx == NULL) || (pStartConfig == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspVideoDecStart.NumStream = NumStream;
            Rval = dsp_wrapper_osal_memcpy(DspVideoDecStart.StreamIdx, pStreamIdx, sizeof(UINT16)*NumStream);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(DspVideoDecStart.StartCfg, pStartConfig, sizeof(AMBA_DSP_VIDDEC_START_CONFIG_s)*NumStream);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            retcode = ioctl(dsp_fd, AMBADSP_VIDEO_DEC_START, &DspVideoDecStart);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VideoDecStop(UINT16 NumStream, const UINT16 *pStreamIdx, const UINT8 *pShowLastFrame)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_video_dec_stop_t DspVideoDecStop = {0};

    /* ioctl */
    if ((pStreamIdx == NULL) || (pShowLastFrame == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspVideoDecStop.NumStream = NumStream;
            Rval = dsp_wrapper_osal_memcpy(DspVideoDecStop.StreamIdx, pStreamIdx, sizeof(UINT16)*NumStream);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(DspVideoDecStop.ShowLastFrame, pShowLastFrame, sizeof(UINT8)*NumStream);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            retcode = ioctl(dsp_fd, AMBADSP_VIDEO_DEC_STOP, &DspVideoDecStop);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VideoDecTrickPlay(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_TRICKPLAY_s *pTrickPlay)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_video_dec_trickplay_t DspVideoDecTrickplay = {0};

    /* ioctl */
    if ((pStreamIdx == NULL) || (pTrickPlay == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspVideoDecTrickplay.NumStream = NumStream;
            Rval = dsp_wrapper_osal_memcpy(DspVideoDecTrickplay.StreamIdx, pStreamIdx, sizeof(UINT16)*NumStream);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(DspVideoDecTrickplay.TrickPlay, pTrickPlay, sizeof(AMBA_DSP_VIDDEC_TRICKPLAY_s)*NumStream);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            retcode = ioctl(dsp_fd, AMBADSP_VIDEO_DEC_TRICKPLAY, &DspVideoDecTrickplay);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VideoDecBitsFifoUpdate(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_BITS_FIFO_s *pBitsFifo)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    uint32_t i;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_video_dec_bits_update_t DspVideoDecBitsUpdate = {0};
    ULONG phys_addr;

    /* ioctl */
    if ((pStreamIdx == NULL) || (pBitsFifo == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspVideoDecBitsUpdate.NumStream = NumStream;
            Rval = dsp_wrapper_osal_memcpy(DspVideoDecBitsUpdate.StreamIdx, pStreamIdx, sizeof(UINT16)*NumStream);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(DspVideoDecBitsUpdate.BitsFifo, pBitsFifo, sizeof(AMBA_DSP_VIDDEC_BITS_FIFO_s)*NumStream);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            /* virt2phys */
            for (i = 0U; i < NumStream; i++) {
                Rval = dsp_wrapper_osal_virt2phys(DspVideoDecBitsUpdate.BitsFifo[i].StartAddr, &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspVideoDecBitsUpdate.BitsFifo[i].StartAddr = phys_addr;

                Rval = dsp_wrapper_osal_virt2phys(DspVideoDecBitsUpdate.BitsFifo[i].EndAddr, &phys_addr);
                dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
                DspVideoDecBitsUpdate.BitsFifo[i].EndAddr = phys_addr;
            }

            retcode = ioctl(dsp_fd, AMBADSP_VIDEO_DEC_BITS_UPDATE, &DspVideoDecBitsUpdate);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_VideoDecPostCtrl(UINT16 StreamIdx, UINT16 NumPostCtrl, const AMBA_DSP_VIDDEC_POST_CTRL_s *pPostCtrl)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_video_dec_post_ctrl_t DspVideoDecPostCtrl = {0};

    /* ioctl */
    if (pPostCtrl == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspVideoDecPostCtrl.StreamIdx = StreamIdx;
            DspVideoDecPostCtrl.NumPostCtrl = NumPostCtrl;
            Rval = dsp_wrapper_osal_memcpy(DspVideoDecPostCtrl.PostCtrl, pPostCtrl, sizeof(AMBA_DSP_VIDDEC_POST_CTRL_s)*NumPostCtrl);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            retcode = ioctl(dsp_fd, AMBADSP_VIDEO_DEC_POST_CTRL, &DspVideoDecPostCtrl);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_StillDecStart(UINT16 StreamIdx, const AMBA_DSP_STLDEC_START_CONFIG_s *pDecConfig)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_still_dec_start_t DspStillDecStart = {0};
    ULONG phys_addr;

    /* ioctl */
    if (pDecConfig == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspStillDecStart.StreamIdx = StreamIdx;
            Rval = dsp_wrapper_osal_memcpy(&DspStillDecStart.DecConfig, pDecConfig, sizeof(AMBA_DSP_STLDEC_START_CONFIG_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            /* virt2phys */
            Rval = dsp_wrapper_osal_virt2phys(DspStillDecStart.DecConfig.BitsAddr, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStillDecStart.DecConfig.BitsAddr = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStillDecStart.DecConfig.YuvBufAddr, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStillDecStart.DecConfig.YuvBufAddr = phys_addr;

            retcode = ioctl(dsp_fd, AMBADSP_STILL_DEC_START, &DspStillDecStart);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_StillDecStop(void)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();

    /* ioctl */
    if (dsp_fd > 0) {
        retcode = ioctl(dsp_fd, AMBADSP_STILL_DEC_STOP);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

UINT32 AmbaDSP_StillDecYuv2Yuv(const AMBA_DSP_YUV_IMG_BUF_s *pSrcYuvBufAddr,
                               const AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                               const AMBA_DSP_STLDEC_YUV2YUV_s *pOperation)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_still_dec_y2y_t DspStillDecY2Y = {0};
    ULONG phys_addr;

    /* ioctl */
    if ((pSrcYuvBufAddr == NULL) || (pDestYuvBufAddr == NULL) || (pOperation == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            Rval = dsp_wrapper_osal_memcpy(&DspStillDecY2Y.SrcYuvBufAddr, pSrcYuvBufAddr, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspStillDecY2Y.DestYuvBufAddr, pDestYuvBufAddr, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspStillDecY2Y.Operation, pOperation, sizeof(AMBA_DSP_STLDEC_YUV2YUV_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            /* virt2phys */
            Rval = dsp_wrapper_osal_virt2phys(DspStillDecY2Y.SrcYuvBufAddr.BaseAddrY, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStillDecY2Y.SrcYuvBufAddr.BaseAddrY = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStillDecY2Y.SrcYuvBufAddr.BaseAddrUV, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStillDecY2Y.SrcYuvBufAddr.BaseAddrUV = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStillDecY2Y.DestYuvBufAddr.BaseAddrY, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStillDecY2Y.DestYuvBufAddr.BaseAddrY = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStillDecY2Y.DestYuvBufAddr.BaseAddrUV, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStillDecY2Y.DestYuvBufAddr.BaseAddrUV = phys_addr;

            retcode = ioctl(dsp_fd, AMBADSP_STILL_DEC_Y2Y, &DspStillDecY2Y);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_StillDecYuvBlend(const AMBA_DSP_YUV_IMG_BUF_s *pSrc1YuvBufAddr,
                                const AMBA_DSP_YUV_IMG_BUF_s *pSrc2YuvBufAddr,
                                const AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                                const AMBA_DSP_STLDEC_BLEND_s *pOperation)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_still_dec_blend_t DspStillDecBlend = {0};
    ULONG phys_addr;

    /* ioctl */
    if ((pSrc1YuvBufAddr == NULL) || (pSrc2YuvBufAddr == NULL) || (pDestYuvBufAddr == NULL) || (pOperation == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            Rval = dsp_wrapper_osal_memcpy(&DspStillDecBlend.Src1YuvBufAddr, pSrc1YuvBufAddr, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspStillDecBlend.Src2YuvBufAddr, pSrc2YuvBufAddr, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspStillDecBlend.DestYuvBufAddr, pDestYuvBufAddr, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspStillDecBlend.Operation, pOperation, sizeof(AMBA_DSP_STLDEC_BLEND_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            /* virt2phys */
            Rval = dsp_wrapper_osal_virt2phys(DspStillDecBlend.Src1YuvBufAddr.BaseAddrY, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStillDecBlend.Src1YuvBufAddr.BaseAddrY = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStillDecBlend.Src1YuvBufAddr.BaseAddrUV, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStillDecBlend.Src1YuvBufAddr.BaseAddrUV = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStillDecBlend.Src2YuvBufAddr.BaseAddrY, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStillDecBlend.Src2YuvBufAddr.BaseAddrY = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStillDecBlend.Src2YuvBufAddr.BaseAddrUV, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStillDecBlend.Src2YuvBufAddr.BaseAddrUV = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStillDecBlend.DestYuvBufAddr.BaseAddrY, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStillDecBlend.DestYuvBufAddr.BaseAddrY = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStillDecBlend.DestYuvBufAddr.BaseAddrUV, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStillDecBlend.DestYuvBufAddr.BaseAddrUV = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStillDecBlend.Operation.AlphaMapAddr, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStillDecBlend.Operation.AlphaMapAddr = phys_addr;

            retcode = ioctl(dsp_fd, AMBADSP_STILL_DEC_BLEND, &DspStillDecBlend);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_StillDecDispYuvImg(UINT8 VoutIdx,
                                  const AMBA_DSP_YUV_IMG_BUF_s *pYuvBufAddr,
                                  const AMBA_DSP_VOUT_VIDEO_CFG_s *pVoutConfig)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_still_dec_disp_yuv_t DspStillDecDispYuv = {0};
    ULONG phys_addr;

    /* ioctl */
    if ((pYuvBufAddr == NULL) || (pVoutConfig == NULL)) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            DspStillDecDispYuv.VoutIdx = VoutIdx;
            Rval = dsp_wrapper_osal_memcpy(&DspStillDecDispYuv.YuvBufAddr, pYuvBufAddr, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            Rval = dsp_wrapper_osal_memcpy(&DspStillDecDispYuv.VoutConfig, pVoutConfig, sizeof(AMBA_DSP_VOUT_VIDEO_CFG_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);

            /* virt2phys */
            Rval = dsp_wrapper_osal_virt2phys(DspStillDecDispYuv.YuvBufAddr.BaseAddrY, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStillDecDispYuv.YuvBufAddr.BaseAddrY = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStillDecDispYuv.YuvBufAddr.BaseAddrUV, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStillDecDispYuv.YuvBufAddr.BaseAddrUV = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStillDecDispYuv.VoutConfig.DefaultImgConfig.BaseAddrY, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStillDecDispYuv.VoutConfig.DefaultImgConfig.BaseAddrY = phys_addr;

            Rval = dsp_wrapper_osal_virt2phys(DspStillDecDispYuv.VoutConfig.DefaultImgConfig.BaseAddrUV, &phys_addr);
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            DspStillDecDispYuv.VoutConfig.DefaultImgConfig.BaseAddrUV = phys_addr;

            retcode = ioctl(dsp_fd, AMBADSP_STILL_DEC_DISP_YUV, &DspStillDecDispYuv);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

#if defined (CONFIG_ENABLE_DSP_DIAG)
UINT32 AmbaDSP_DiagCase(UINT32 DiagId)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();

    if (dsp_fd > 0) {
        retcode = ioctl(dsp_fd, AMBADSP_DIAG_CASE, &DiagId);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}
#endif

#ifdef CONFIG_ENABLE_DSP_MONITOR
UINT32 AmbaDSP_MonitorInit(void)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();

    /* ioctl */
    if (dsp_fd > 0) {
        retcode = ioctl(dsp_fd, AmbaDSP_MONITOR_INIT);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
    }

    return Rval;
}

UINT32 AmbaDSP_MonitorErrorNotify(const AMBA_DSP_MONITOR_ERR_NOTIFY_s *pErrNotify)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_mon_err_notify_t DspMonErrNotify = {0};

    /* ioctl */
    if (pErrNotify == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            Rval = dsp_wrapper_osal_memcpy(&DspMonErrNotify.ErrNtfy, pErrNotify, sizeof(AMBA_DSP_MONITOR_ERR_NOTIFY_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            retcode = ioctl(dsp_fd, AmbaDSP_MONITOR_ERR_NOTIFY, &DspMonErrNotify);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_MonitorCrcCmpr(const AMBA_DSP_MONITOR_CRC_CMPR_s *pCrcCmpr)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_mon_crc_cmpr_t DspMonCrcCmpr = {0};

    /* ioctl */
    if (pCrcCmpr == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            Rval = dsp_wrapper_osal_memcpy(&DspMonCrcCmpr.CrcCmpr, pCrcCmpr, sizeof(AMBA_DSP_MONITOR_CRC_CMPR_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            retcode = ioctl(dsp_fd, AmbaDSP_MONITOR_CRC_CMPR, &DspMonCrcCmpr);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

UINT32 AmbaDSP_GetMonitorConfig(UINT32 ModuleId, AMBA_DSP_MONITOR_CONFIG_s *pMonCfg)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_mon_get_cfg_t DspMonGetCfg = {0};

    /* ioctl */
    if (dsp_fd > 0) {
        DspMonGetCfg.ModuleId = ModuleId;
        retcode = ioctl(dsp_fd, AmbaDSP_MONITOR_GET_CFG, &DspMonGetCfg);
        Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        if (pMonCfg != NULL) {
            pMonCfg->BitMask = DspMonGetCfg.MonitorCfg.BitMask;
        }
    }

    return Rval;
}

UINT32 AmbaDSP_MonitorHeartBeatCfg(const AMBA_DSP_MONITOR_HEARTBEAT_s *pHeartBeatCfg)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_mon_heartbeat_cfg_t DspMonHeartBeatCfg = {0};

    /* ioctl */
    if (pHeartBeatCfg == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (dsp_fd > 0) {
            Rval = dsp_wrapper_osal_memcpy(&DspMonHeartBeatCfg.HearBeatCfg, pHeartBeatCfg, sizeof(AMBA_DSP_MONITOR_HEARTBEAT_s));
            dsp_wrapper_osal_print_err(Rval, __func__, __LINE__);
            retcode = ioctl(dsp_fd, AmbaDSP_MONITOR_HEARTBEAT_CFG, &DspMonHeartBeatCfg);
            Rval = dsp_wrap_get_ioctl_rval(retcode, __func__);
        }
    }

    return Rval;
}

#endif

UINT32 AmbaDSP_ClockUpdate(UINT32 ClockId, UINT32 Stage, UINT32 Freq)
{
    uint32_t Rval = DSP_ERR_NONE;
    int32_t retcode = 0;
    int32_t dsp_fd = DSP_GET_FD();
    dsp_sys_drv_cfg_t DspSysDrvCfg = {0};

    if (dsp_fd > 0) {
        if (ClockId == AMBA_DSP_CLOCK_ID_CORE) {
            if (Stage == AMBA_DSP_CLOCK_STAGE_PREPROC) {
                DspSysDrvCfg.Type = DSP_SYS_DRV_CFG_TYPE_CORE_CLK_PRE;
            } else {
                DspSysDrvCfg.Type = DSP_SYS_DRV_CFG_TYPE_CORE_CLK;
            }
            DspSysDrvCfg.SysDrvCfg.Data = Freq;
            retcode = ioctl(dsp_fd, AMBADSP_SYS_DRV_CFG, &DspSysDrvCfg);
        }
    }

    if (retcode < 0) {
        Rval = DSP_ERR_0003;
        dsp_wrapper_osal_printI5("[ERR] AmbaDSP_ClockUpdate : ioctl fail errno %d", errno, 0, 0, 0, 0);
    }
    return Rval;
}

