/**
 *  @file ambarella_hdmi.h
 *
 *  @copyright Copyright (c) 2021 Ambarella International LP
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
 *  @details lib header for HDMI device module
 *
 */

#ifndef _HDMI_LIB_H_INCLUDED
#define _HDMI_LIB_H_INCLUDED

#include <AmbaHDMI.h>

#define AMBA_HDMI_DATA_BUF_SIZE  512U

typedef struct _hdmi_port_msg {
    UINT32                      HdmiPort;
} hdmi_port_msg_t;

typedef struct _hdmi_config_msg {
    UINT32                      HdmiPort;
    AMBA_HDMI_VIDEO_CONFIG_s    VideoConfig;
    AMBA_HDMI_AUDIO_CONFIG_s    AudioConfig;
    AMBA_HDMI_VIDEO_CONFIG_s    WorkingVideoConfig;
    UINT32                      AudioCfgIsNull;
    UINT32                      IsSuccessor;
} hdmi_config_msg_t;

typedef struct _hdmi_sinkinfo_msg {
    UINT32                      HdmiPort;
    UINT32                      HotPlugDetect;
    UINT32                      EdidDetect;
    AMBA_HDMI_SINK_INFO_s       SinkInfo;
} hdmi_sinkinfo_msg_t;

typedef struct _hdmi_modeinfo_msg {
    UINT32                      HdmiPort;
    AMBA_HDMI_MODE_INFO_s       ModeInfo;
    UINT8                       StrictFrameRate;
    UINT16                      FrameWidth;
    UINT16                      FrameHeight;
} hdmi_modeinfo_msg_t;

typedef struct _hdmi_phy_msg {
    UINT32                      HdmiPort;
    UINT32                      ParamID;
    UINT32                      ParamVal;
} hdmi_phy_msg_t;

typedef struct _hdmi_sourceinfo_msg {
    UINT32                          HdmiPort;
    AMBA_HDMI_SOURCE_PRODUCT_INFO_s SrcProductInfo;
} hdmi_sourceinfo_msg_t;

typedef struct _hdmi_infoframe_msg {
    UINT32                          HdmiPort;
    UINT32                          InfoFrameType;
    AMBA_HDMI_DATA_ISLAND_PACKET_s  InfoFramePkt;
    UINT32                          TxMode;
} hdmi_infoframe_msg_t;

typedef struct _hdmi_hpd_isr_msg {
    UINT32                          HdmiPort;
    AMBA_HDMI_HPD_ISR_f             IntHandler;
} hdmi_hpd_isr_msg_t;

typedef union _hdmi_disp_config_msg_t {
    UINT32                          HdmiPort;
    UINT8         Out[AMBA_HDMI_DATA_BUF_SIZE];
} hdmi_disp_config_msg_t;

typedef struct _hdmi_audio_msg {
    UINT32                      AudioSrc;
    UINT32                      AudioClk;
} hdmi_audio_msg_t;

/*
 * The following ioctl commands are used by a client application
 * to control the vin interface.
 */
#include <linux/ioctl.h>

#define HDMI_IOCTL_MAGIC                'h'

#define HDMI_IOC_TX_ENABLE             _IOW(HDMI_IOCTL_MAGIC, 0, hdmi_port_msg_t)
#define HDMI_IOC_TX_DISABLE            _IOW(HDMI_IOCTL_MAGIC, 1, hdmi_port_msg_t)
#define HDMI_IOC_TX_GET_SINKINFO       _IOWR(HDMI_IOCTL_MAGIC, 2, hdmi_sinkinfo_msg_t)
#define HDMI_IOC_TX_SET_MODE           _IOWR(HDMI_IOCTL_MAGIC, 3, hdmi_config_msg_t)
#define HDMI_IOC_TX_GET_CURR_MODE      _IOWR(HDMI_IOCTL_MAGIC, 4, hdmi_modeinfo_msg_t)
#define HDMI_IOC_TX_SET_PHY            _IOW(HDMI_IOCTL_MAGIC, 5, hdmi_phy_msg_t)
#define HDMI_IOC_TX_START              _IOW(HDMI_IOCTL_MAGIC, 6, hdmi_port_msg_t)
#define HDMI_IOC_TX_STOP               _IOW(HDMI_IOCTL_MAGIC, 7, hdmi_port_msg_t)
#define HDMI_IOC_TX_SET_SOURCEINFO     _IOW(HDMI_IOCTL_MAGIC, 8, hdmi_sourceinfo_msg_t)
#define HDMI_IOC_TX_SET_INFOFRAME      _IOW(HDMI_IOCTL_MAGIC, 9, hdmi_infoframe_msg_t)
#define HDMI_IOC_TX_GET_INFOFRAME      _IOWR(HDMI_IOCTL_MAGIC, 10, hdmi_infoframe_msg_t)
#define HDMI_IOC_TX_SET_AVMUTE         _IOW(HDMI_IOCTL_MAGIC, 11, hdmi_port_msg_t)
#define HDMI_IOC_TX_CLEAR_AVMUTE       _IOW(HDMI_IOCTL_MAGIC, 12, hdmi_port_msg_t)
#define HDMI_IOC_TX_GET_DISP_CONFIG    _IOWR(HDMI_IOCTL_MAGIC, 13, hdmi_disp_config_msg_t)
#define HDMI_IOC_TX_ENABLE_TESTPATTERN _IOW(HDMI_IOCTL_MAGIC, 14, hdmi_port_msg_t)
#define HDMI_IOC_TX_SET_AUDIO_SRC      _IOW(HDMI_IOCTL_MAGIC, 15, hdmi_audio_msg_t)
#endif

