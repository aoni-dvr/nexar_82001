################################################################################
 #  app/hostapi/target/make.inc
 #
 # History:
 #   July 27, 2018 - [Jiankun Li] Created file
 #
 # Copyright (c) 2018 Ambarella, Inc.
 #
 # This file and its contents ( "Software" ) are protected by intellectual
 # property rights including, without limitation, U.S. and/or foreign
 # copyrights. This Software is also the confidential and proprietary
 # information of Ambarella, Inc. and its licensors. You may not use, reproduce,
 # disclose, distribute, modify, or otherwise prepare derivative works of this
 # Software or any portion thereof except pursuant to a signed license agreement
 # or nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 # In the absence of such an agreement, you agree to promptly notify and return
 # this Software to Ambarella, Inc.
 #
 # THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 # INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 # MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 # IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 # INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 # (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 # LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 # INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 # CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 # ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 # POSSIBILITY OF SUCH DAMAGE.
 #
################################################################################

LOCAL_PATH	:= $(call my-dir)

###
include $(CLEAR_VARS)

HOSTAPI_TARGET_DIR	:= $(FAKEROOT_DIR)/usr/local/bin/

LOCAL_TARGET	:= hostapid
LOCAL_CFLAGS    += -I$(LOCAL_PATH)/../common/inc -I$(LOCAL_PATH)/inc
LOCAL_LDFLAGS   += -lpthread

LOCAL_SRCS	:= $(LOCAL_PATH)/ambahostd/main.c
LOCAL_SRCS      += $(LOCAL_PATH)/core/host_api.c
LOCAL_SRCS      += $(LOCAL_PATH)/network/tcpip_linux/tcpip_linux.c

include $(BUILD_APP)

.PHONY: $(LOCAL_TARGET)

$(LOCAL_TARGET): $(LOCAL_MODULE)
	@mkdir -p $(HOSTAPI_TARGET_DIR)/
	@cp -dpRf $< $(HOSTAPI_TARGET_DIR)/ambahostd
	@echo "Build $@ Done."

$(call add-target-into-build, $(LOCAL_TARGET))