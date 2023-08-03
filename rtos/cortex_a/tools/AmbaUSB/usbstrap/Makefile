##
## Copyright (c) 2021 Ambarella International LP
##
## This file and its contents ("Software") are protected by intellectual
## property rights including, without limitation, U.S. and/or foreign
## copyrights. This Software is also the confidential and proprietary
## information of Ambarella International LP and its licensors. You may not use, reproduce,
## disclose, distribute, modify, or otherwise prepare derivative works of this
## Software or any portion thereof except pursuant to a signed license agreement
## or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
## In the absence of such an agreement, you agree to promptly notify and return
## this Software to Ambarella International LP
##
## THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
## INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
## MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
## IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
## INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
## (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUDLBEEITUTE GOODS OR SERVICES;
## LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
## INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
## CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
## POSSIBILITY OF SUCH DAMAGE.
##

define addhex
$(eval __x:=$(shell printf "%d" $1))		\
	$(eval __y:=$(shell printf "%d" $2))		\
	$(eval __z:=$(shell expr $(__x) + $(__y)))	\
	$(eval rval:=$(shell printf "0x%x" $(__z)))	\
	$(shell echo $(rval))
endef

define subhex
$(eval __x:=$(shell printf "%d" $1))		\
	$(eval __y:=$(shell printf "%d" $2))		\
	$(eval __z:=$(shell expr $(__x) - $(__y)))	\
	$(eval rval:=$(shell printf "0x%x" $(__z)))	\
	$(shell echo $(rval))
endef

LOCAL_OUTPUT	?= $(shell pwd)/out
INSTALL_PATH	?= $(shell pwd)/install
SOC		?= CV5
IMAGE_VERSION	?= 0
IMAGE_KEYID	?= 0
IMAGE_SIGNED	?= 0
IMAGE_PREFIX	?= none
DRAM_TYPE	?= lpddr5
DRAM_CFG	?= none
BSTINITPARSE	?= tools/bstiniparser
INI_FILE	?=

CROSS_COMPILE	?= unknown
ED25519_PRIVKEY	?= keys/ed25519/priv.pem

AR		:= $(CROSS_COMPILE)ar
CC		:= $(CROSS_COMPILE)gcc
CPP		:= $(CROSS_COMPILE)gcc -E
LD		:= $(CROSS_COMPILE)ld
NM		:= $(CROSS_COMPILE)nm
OBJCOPY		:= $(CROSS_COMPILE)objcopy
OBJDUMP		:= $(CROSS_COMPILE)objdump


AMBUST_SRAM_ADDR	:= 0x20E0031000
AMBUST_SRAM_SIZE	:= 0x0000003000

# MUST be 64Bytes for ED25519
IMAGE_SIGN_SIZE	:= 64

# Unsigned area.
IMAGE_RSVD_SIZE	:= 192

# Pad the output file up to the load address.
# TEXT_PAD = AMBUST_SRAM_ADDR + AMBUST_SRAM_SIZE - IMAGE_SIGN_SIZE - IMAGE_RSVD_SIZE
TEXT_PAD	:=$(call subhex, \
		$(call subhex,	\
		$(call addhex, $(AMBUST_SRAM_ADDR), $(AMBUST_SRAM_SIZE)), \
		$(IMAGE_SIGN_SIZE)), \
		$(IMAGE_RSVD_SIZE))

DATE		:= $(shell date -R)
EXTRA_FLAGS	:= -DSRAM_ADDR=$(AMBUST_SRAM_ADDR)
EXTRA_FLAGS	+= -DSRAM_SIZE=$(AMBUST_SRAM_SIZE)
EXTRA_FLAGS	+= -D__BUILD_DATE__="\"$(DATE)\""
EXTRA_FLAGS	+= -DIMAGE_SIGN_SIZE=$(IMAGE_SIGN_SIZE)
EXTRA_FLAGS	+= -DIMAGE_RSVD_SIZE=$(IMAGE_RSVD_SIZE)
EXTRA_FLAGS	+= -DVERSION=$(IMAGE_VERSION)
EXTRA_FLAGS	+= -DKEYID=$(IMAGE_KEYID)
EXTRA_FLAGS	+= -DCONFIG_ARCH_$(SOC)
EXTRA_FLAGS	+= -DBYPASS_DRAM_INIT=0
EXTRA_FLAGS	+= -DDEBUG=0

ifeq ($(DRAM_TYPE), lpddr5)
EXTRA_FLAGS	+= -DDRAM_TYPE_LPDDR5
endif

ifeq ($(DRAM_TYPE), lpddr4)
EXTRA_FLAGS	+= -DDRAM_TYPE_LPDDR4
endif

ifeq ($(PROTOCOL), ambusb)
EXTRA_FLAGS	+= -DCONFIG_AMBUSB
endif

INCLUDES	:= -I include -I $(LOCAL_OUTPUT)
LDFLAGS		:= $(EXTRA_FLAGS) $(INCLUDES) -Wl,--gc-sections -Wl,--build-id=none -nostdlib
AFLAGS		:= $(EXTRA_FLAGS) $(INCLUDES) -D__ASM__ -nostdinc
CFLAGS		:= $(EXTRA_FLAGS) $(INCLUDES) \
			-Wall	\
			-ffunction-sections -O3 \
			-nostdinc -fno-builtin \
			-mstrict-align \
			-mgeneral-regs-only

SOC_LO		:= $(shell echo $(SOC) | tr [:upper:] [:lower:])

AMBUST_ASM_OBJ	= @mkdir -p $(dir $@) && $(CC) $(AFLAGS) -c $< -o $@
AMBUST_C_OBJ	= @mkdir -p $(dir $@) && $(CC) $(CFLAGS) -c $< -o $@

ifeq ($(IMAGE_SIGNED),1)
CREATE_SIGN	?= openssl pkeyutl -sign -inkey $(ED25519_PRIVKEY) -out $@ -rawin -in $<
else
#CREATE_SIGN	?= fallocate -l $(IMAGE_SIGN_SIZE) $@
CREATE_SIGN	?= dd if=/dev/zero of=$@ count=1 bs=$(IMAGE_SIGN_SIZE)
endif

.SECONDARY:
OBJS		:= \
		$(LOCAL_OUTPUT)/entry.o \
		$(LOCAL_OUTPUT)/main.o \
		$(LOCAL_OUTPUT)/uart.o \
		$(LOCAL_OUTPUT)/misc.o \
		$(LOCAL_OUTPUT)/common.o \
		$(LOCAL_OUTPUT)/printf.o \
		$(LOCAL_OUTPUT)/usb_device.o \
		$(LOCAL_OUTPUT)/fastboot.o \
		$(LOCAL_OUTPUT)/ambdl.o \

include $(SOC_LO)/Makefile

all: $(LOCAL_OUTPUT)/ambust_$(DRAM_TYPE)_signed.bin
	@

# AMBUST target
$(LOCAL_OUTPUT)/ambust_%_signed.bin: $(LOCAL_OUTPUT)/ambust_%.bin $(LOCAL_OUTPUT)/ambust_%_rsvd.bin $(LOCAL_OUTPUT)/ambust_%.sign
	@cat $(filter %.bin %.sign, $^) > $@

$(LOCAL_OUTPUT)/ambust_%.sign: $(LOCAL_OUTPUT)/ambust_%.bin
	@$(CREATE_SIGN)

$(LOCAL_OUTPUT)/ambust_$(DRAM_TYPE)_rsvd.bin:
	@dd if=/dev/zero of=$@ count=1 bs=$(IMAGE_RSVD_SIZE)

$(LOCAL_OUTPUT)/ambust_$(DRAM_TYPE).bin: $(LOCAL_OUTPUT)/ambust_$(DRAM_TYPE).elf
	@$(OBJCOPY) -j .text -j .data -S -g -O binary --pad-to $(TEXT_PAD) $< $@

$(LOCAL_OUTPUT)/ambust_$(DRAM_TYPE).elf: $(LOCAL_OUTPUT)/ust.lds $(OBJS)
	@$(CC) -Wl,-T,$< -nostartfiles $(OBJS) $(LDFLAGS) -o $@
	@$(NM) -n $@ | grep -v '\( [aUw] \)\|\(__crc__\)' > $(subst .elf,.map,$@)
	@$(OBJDUMP) -dx $@ > $(subst .elf,.dump,$@)

$(LOCAL_OUTPUT)/ust.lds: ust.lds.cpp
	@mkdir -p $(dir $@)
	@$(CPP) $(INCLUDES) $(EXTRA_FLAGS) $< | sed /^#/d > $@

$(LOCAL_OUTPUT)/%.o: $(SOC_LO)/%.S
	@$(AMBUST_ASM_OBJ)

$(LOCAL_OUTPUT)/%.o: %.S
	@$(AMBUST_ASM_OBJ)

$(LOCAL_OUTPUT)/%.o: $(SOC_LO)/%.c
	@$(AMBUST_C_OBJ)

$(LOCAL_OUTPUT)/%.o: %.c
	@$(AMBUST_C_OBJ)

install: $(LOCAL_OUTPUT)/ambust_$(DRAM_TYPE)_signed.bin
	@mkdir -p $(INSTALL_PATH)
	@install -m 755 $< $(INSTALL_PATH)/$(IMAGE_PREFIX)_ambust_$(DRAM_TYPE)_signed.bin

clean:
	@mkdir -p $(LOCAL_OUTPUT)
	@cd $(LOCAL_OUTPUT) && rm -rf *

parameter:
	@if [ "$(DRAM_CFG)" = "" ]; then \
		echo >&2 "***"; \
		echo >&2 "*** 'DRAM_CFG' is not specified"; \
		echo >&2 "***"; \
		false ; \
	fi
	@if [ ! -e $(DRAM_CFG) ]; then \
		echo >&2 "***"; \
		echo >&2 "*** $(DRAM_CFG): No such file or directory"; \
		echo >&2 "***"; \
		false; \
	else \
		echo "Build USBstrap ($(PROTOCOL)) with $(DRAM_CFG)"; \
	fi
	@mkdir -p $(LOCAL_OUTPUT) \
		&& $(BSTINITPARSE) $(DRAM_CFG) > $(LOCAL_OUTPUT)/parameters.h
