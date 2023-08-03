/**
 *  @file StdUAC.h
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
 *  @details Standard USB Audio Class header file.
 */

#ifndef STD_USB_AUDIO_CLASS_H
#define STD_USB_AUDIO_CLASS_H

// ----------------------------------------
// UAC device class
// ----------------------------------------
#define USB_DEVICE_CLASS_CODE_UAC     0xEFU
#define USB_DEVICE_SUBCLASS_CODE_UAC  0x02U
#define USB_DEVICE_PROTOCOL_CODE_UAC  0x01U

// UAC Interface class code
#define UAC_INTF_CLASS_CODE_AUDIO                 0x01U

// UAC Interface Subclass code
#define UAC_INTF_SUBCLASS_CODE_AUDIO_CONTROL      0x01U
#define UAC_INTF_SUBCLASS_CODE_AUDIO_STREAM       0x02U
#define UAC_INTF_SUBCLASS_CODE_MIDI_STREAM        0x03U
#define UAC_INTF_SUBCLASS_CODE_IAD                0x02U

// UAC Interface protocol code
#define UAC_INTF_PROTOCOL_CODE_UNDEFINE           0x00U
#define UAC_INTF_PROTOCOL_CODE_IP_VERSION_02_00   0x20U // UAC20

// UAC Descriptor Length
#define UAC_DESC_LENGTH_CS_AC_INTF                0x09U
#define UAC_DESC_LENGTH_AC_INPUT_TERMINAL         0x0CU
#define UAC_DESC_LENGTH_AC_OUTPUT_TERMINAL        0x09U
#define UAC_DESC_LENGTH_AC_FEATURE_UNIT           0x09U
#define UAC_DESC_LENGTH_CS_AS_INTF                0x07U
#define UAC_DESC_LENGTH_TYPE_I_FORMAT             0x0BU
#define UAC_DESC_LENGTH_AS_ENDPOINT               0x09U
#define UAC_DESC_LENGTH_CS_ENDPOINT               0x07U

// UAC Class-Specific Endpoint Descriptor SubType
#define UAC_DESC_SUBTYPE_CS_EP_GENERAL            0x01U

// UAC Terminal Type
#define UAC_TERMINAL_TYPE_STREAMING               0x0101U
#define UAC_TERMINAL_TYPE_VENDOR_SPECIFIC         0x01FFU

// UAC Input Terminal Type
#define UAC_INPUT_TERMINAL_TYPE_MICROPHONE        0x0201U

// UAC Feature Unit Control Selectors
#define UAC_FU_CONTROL_UNDEFINED                  0x00U
#define UAC_FU_MUTE_CONTROL                       0x01U
#define UAC_FU_VOLUME_CONTROL                     0x02U
#define UAC_FU_BASS_CONTROL                       0x03U
#define UAC_FU_MID_CONTROL                        0x04U
#define UAC_FU_TREBLE_CONTROL                     0x05U
#define UAC_FU_GRAPHIC_EQUALIZER_CONTROL          0x06U
#define UAC_FU_AUTOMATIC_GAIN_CONTROL             0x07U
#define UAC_FU_DELAY_CONTROL                      0x08U
#define UAC_FU_BASS_BOOST_CONTROL                 0x09U
#define UAC_FU_LOUDNESS_CONTROL                   0x0AU

// Audio Class-Specific Request Code
#define UAC_SET_CUR         0x01U
#define UAC_GET_CUR         0x81U
#define UAC_SET_MIN         0x02U
#define UAC_GET_MIN         0x82U
#define UAC_SET_MAX         0x03U
#define UAC_GET_MAX         0x83U
#define UAC_SET_RES         0x04U
#define UAC_GET_RES         0x84U
#define UAC_SET_MEM         0x05U
#define UAC_GET_MEM         0x85U
#define UAC_GET_STAT        0xFFU

#define UAC_TYPE_UNDEFINED 0x00
#define UAC_TYPE_I         0x01
#define UAC_TYPE_II        0x02
#define UAC_TYPE_III       0x03

#define UAC_TYPE_I_UNDEFINED  0x0000
#define UAC_TYPE_I_PCM        0x0001
#define UAC_TYPE_I_PCM8       0x0002
#define UAC_TYPE_I_IEEE_FLOAT 0x0003
#define UAC_TYPE_I_ALAW       0x0004
#define UAC_TYPE_I_MULAW      0x0005

// UAC AC Interface Descriptor SubType
#define UAC_SUBTYPE_AC_UNDEFINED            0x00U
#define UAC_SUBTYPE_AC_HEADER               0x01U
#define UAC_SUBTYPE_AC_INPUT_TERMINAL       0x02U
#define UAC_SUBTYPE_AC_OUTPUT_TERMINAL      0x03U
#define UAC_SUBTYPE_AC_MIXER_UNIT           0x04U
#define UAC_SUBTYPE_AC_SELECTOR_UNIT        0x05U
#define UAC_SUBTYPE_AC_FEATURE_UNIT         0x06U
#define UAC_SUBTYPE_AC_PROCESSING_UNIT      0x07U
#define UAC_SUBTYPE_AC_EXTENSION_UNIT       0x08U

// UAC AS Interface Descriptor SubType
#define UAC_SUBTYPE_AS_UNDEFINED            0x00U
#define UAC_SUBTYPE_AS_GENERAL              0x01U
#define UAC_SUBTYPE_AS_FORMAT_TYPE          0x02U
#define UAC_SUBTYPE_AS_FORMAT_SPECIFIC      0x03U

// UAC Descriptor Type
#define UAC_TYPE_CS_UNDEFINED       0x20U
#define UAC_TYPE_CS_DEVICE          0x21U
#define UAC_TYPE_CS_CONFIGURATION   0x22U
#define UAC_TYPE_CS_STRING          0x23U
#define UAC_TYPE_CS_INTERFACE       0x24U
#define UAC_TYPE_CS_ENDPOINT        0x25U

// UAC Endpoint Control Selectors

#define UAC_EP_CONTROL_UNDEFINED            0x00U
#define UAC_EP_SAMPLING_FREQ_CONTROL        0x01U
#define UAC_EP_PITCH_CONTROL                0x02U

#endif
