##
## Copyright (c) 2021 Ambarella, Inc.
##
## This file and its contents ("Software") are protected by intellectual
## property rights including, without limitation, U.S. and/or foreign
## copyrights. This Software is also the confidential and proprietary
## information of Ambarella, Inc. and its licensors. You may not use, reproduce,
## disclose, distribute, modify, or otherwise prepare derivative works of this
## Software or any portion thereof except pursuant to a signed license agreement
## or nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
## In the absence of such an agreement, you agree to promptly notify and return
## this Software to Ambarella, Inc.
##
## THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
## INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
## MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
## IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
## INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
## (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
## LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
## INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
## CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
## POSSIBILITY OF SUCH DAMAGE.
##
LOCAL_PATH		:=$(call my-dir)
LOCAL_OUTPUT		:= $(AMB_BOARD_OUT)/$(strip $(subst $(AMB_TOPDIR),,$(call my-dir)))

DEF_DRAM_TYPE-$(CONFIG_USBSTRAP_DRAM_TYPE_LPDDR4)=lpddr4
DEF_DRAM_TYPE-$(CONFIG_USBSTRAP_DRAM_TYPE_LPDDR5)=lpddr5
DEF_PROTOCOL-$(CONFIG_USBSTRAP_PROTOCOL_AMBUSB)=ambusb
DEF_PROTOCOL-$(CONFIG_USBSTRAP_PROTOCOL_FASTBOOT)=fastboot

AMBA_BSTINIPARSER	?= $(AMBOOT_TOPDIR)/src/bst/tools/bstiniparser
AMBA_USBSTRAP_INI	?= $(shell echo $(CONFIG_USBSTRAP_INI_FILE))

LOCAL_EXPORT		:=  \
			SOC=CV5 \
			IMAGE_VERSION=0 \
			IMAGE_KEYID=0 \
			IMAGE_SIGNED=0 \
			IMAGE_PREFIX=$(AMB_BOARD) \
			LOCAL_OUTPUT=$(LOCAL_OUTPUT) \
			INSTALL_PATH=$(IMAGES_OUT_DIR) \
			BSTINIPARSER=$(AMBA_BSTINIPARSER) \
			DRAM_CFG=$(AMBA_USBSTRAP_INI) \
			DRAM_TYPE=$(DEF_DRAM_TYPE-y) \
			PROTOCOL=$(DEF_PROTOCOL-y)

.PHONY: ambust  ambust_clean

ambust: PRIVATE_PATH:=$(LOCAL_PATH)
ambust: PRIVATE_OUTPUT:=$(LOCAL_OUTPUT)
ambust: PRIVATE_EXPORT:=$(LOCAL_EXPORT)
ambust: $(DOT_CONFIG)
	@$(PRIVATE_EXPORT) $(MAKE) $(AMBA_MAKE_PARA) -C $(PRIVATE_PATH) clean
	@$(PRIVATE_EXPORT) $(MAKE) $(AMBA_MAKE_PARA) -C $(PRIVATE_PATH) parameter
	@$(PRIVATE_EXPORT) $(MAKE) $(AMBA_MAKE_PARA) -C $(PRIVATE_PATH) all
	@$(PRIVATE_EXPORT) $(MAKE) $(AMBA_MAKE_PARA) -C $(PRIVATE_PATH) install

ambust_clean: PRIVATE_PATH:=$(LOCAL_PATH)
ambust_clean: PRIVATE_OUTPUT:=$(LOCAL_OUTPUT)
ambust_clean: $(DOT_CONFIG)
	@rm -rf $(PRIVATE_OUTPUT)
