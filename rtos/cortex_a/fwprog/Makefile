# Print debugging information in addition to normal processing.
#MAKEFLAGS += -d
# --print-directory
#MAKEFLAGS += -w
# --print-data-base
#MAKEFLAGS += -p
# Kbuild Verbose (empty as enabled)
#Q=

#
# 1. $(srctree)/fwprog/gen_amba_fwlist.sh generate $(objtree)/$(obj)/.temp/.fwlist
#    i.e. $(FWPROG_OUT_DIR)/.fwlist
# 2. .fwlist -> FWLIST -> FIRMWARES -> FWPROG
#    i.e. $(FWPROG_OUT_DIR)/*.elf
# 3. %.elf: *.fw AmbaFwProg_%.fw %.lds $(FWPROG_LIBS)
#    3.1 *.fw: input-binary $(src)/AmbaFwImgHeader.S
#        *.fw file is elf with image header that contain image binary.
#    3.2 %.bin: %.fw
#        .bin is used for SD FwProg
#    3.3 AmbaFwProg_%.fw: Header of to describe many *.fw
#    3.4 %.lds: Memory layout
# 4. main() function is ../bsp/Chip/Board/AmbaFlashFwProg_UserSysCtrl.c
#    It is put at libbsp_xxx.a
#
# libfwprog.a: Basic APIs of initialization used by fwprog.
# libfwprog_user.a: For partial load and main function
#
# bst_bld_xxx.elf = AmbaFwProg_bst_bld_xxx.fw + bst_bld_xxx.lds + multiple image.fw
#   AmbaFwProg_%.fw = AmbaFwProgHeader_%.S
#     AmbaFwProgHeader_%.S = $(src)/AmbaFwProgHeader.S with -D__LINK_xxx__, _D__LINK_yyy__, etc.
#   %.lds = $(src)/amba_fwprog.lds.S with -D__LINK_xxx__, _D__LINK_yyy__, etc.
#
# image.fw = REAL_image $(src)/AmbaFwImgHeader.S
#   REAL_image = sign(zip(ABS_image))
#     ABS_image = $(abspath $(CONFIG_FWPROG_image))
#
# Top target is $(FWPROG_OUT_DIR)/.stamp_output: $(FWPROG_LIB) $(FWPROG) $(addprefix $(FWPROG_OUT_DIR)/,$(all_obin))
#   Trigger to generate FwProg library, *.elf, *.fw, *.bin
#
# TODO: zip(ABS_image)
#

KBUILD_CFLAGS := $(subst -fPIE ,,$(KBUILD_CFLAGS))
KBUILD_CFLAGS += -no-pie

LDFLAGS_amba_fwprog := $(LDFLAGS_amba_common)
export LDFLAGS_amba_fwprog
export DIR_amba_fwprog := $(obj)
export DIR_amba_fwprog_out := .temp
export FWPROG_OUT_DIR := $(DIR_amba_fwprog)/$(DIR_amba_fwprog_out)
export LDS_amba_fwprog := $(obj)/$(DIR_amba_fwprog)/amba_fwprog.lds

# ------------------------------------------------------------------ #
# obj-y
# ------------------------------------------------------------------ #

ifneq ($(wildcard $(srctree)/$(src)/src/Makefile),)
# Library used for FwProg
obj-y += src/
endif

# Main function and for partial load.
obj-y   += libfwprog_user.a
libfwprog_user-objs += AmbaFwLoader_RegionInfo.o
ifneq ($(AMBA_BSP_NAME),)
BSP_PATH := $(AMBA_CHIP_ID)/$(AMBA_BSP_NAME)
libfwprog_user-objs += ../bsp/$(BSP_PATH)/AmbaFlashFwProg_UserSysCtrl.o
endif

# Final target
obj-y += $(DIR_amba_fwprog_out)/.stamp_output
$(obj)/$(DIR_amba_fwprog_out)/.stamp_output: $(obj)/src $(obj)/libfwprog_user.a

# ------------------------------------------------------------------ #
# libfwprog_user.a
# ------------------------------------------------------------------ #
ifneq ($(SYS),)
ifneq ($(O),)
ABS_SYS_BIN = $(if $(call if_abs_path,$(SYS)),$(SYS),$(if $(call if_abs_path,$(O)),$(O),$(srctree)/$(O))/$(SYS))
else
ABS_SYS_BIN = $(if $(call if_abs_path,$(SYS)),$(SYS),$(srctree)/$(SYS))
endif # $(O)
ABS_SYS_ELF := $(patsubst %.bin,%.elf,$(ABS_SYS_BIN))
else # No  CONFIG_FWPROG_SYS
ABS_SYS_ELF :=
endif # CONFIG_FWPROG_SYS

# For partial load
$(objtree)/fwprog/AmbaFwLoader_RegionInfo.c: FORCE
	$(Q)chmod 711 $(srctree)/fwprog/gen_RegionInfo.sh
	$(Q)$(srctree)/fwprog/gen_RegionInfo.sh $(ABS_SYS_ELF)

fwprog/AmbaFwLoader_RegionInfo.o: $(objtree)/fwprog/AmbaFwLoader_RegionInfo.c

# ------------------------------------------------------------------ #
# Do not include reent.h
# ------------------------------------------------------------------ #
KBUILD_CFLAGS := $(subst -include reent.h,,$(KBUILD_CFLAGS))
KBUILD_AFLAGS := $(subst -include reent.h,,$(KBUILD_AFLAGS))

ccflags-y += -I$(src)/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/soc/io/src/common/inc
ccflags-y += -I$(srctree)/soc/io/src/common/inc/arm
ccflags-y += -I$(srctree)/soc/io/src/common/inc/rtsl

ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg

# ------------------------------------------------------------------ #
# Final targets
# ------------------------------------------------------------------ #

FWPROG_NAME_PAT=[0-9A-Z]*
# Get all partition targets from CONFIG_FWPROG_NAME
all_PAT = $(shell grep -Inr "CONFIG_FWPROG_$(FWPROG_NAME_PAT)=" $(objtree)/$(KCONFIG_CONFIG) | grep -v '""' | awk -F '=' '{print $$1}' | awk -F '_' '{print $$NF}' | sort)
all_pat = $(shell echo $(all_PAT) | tr '[:upper:]' '[:lower:]')

#
# Macros for variables
#
define NEWLINE

endef

abs_bin = ABS_$(1) = $(abspath $(shell echo $(CONFIG_FWPROG_$(1))))
define real_bin

ifneq ($(CONFIG_ENABLE_SECURITY),)
# secure boot
REAL_$(1) = $(if $(ABS_$(1)), \
		$(if $(CONFIG_FWPROG_$(1)_SIGNED), $(FWPROG_OUT_DIR)/$(1).signed, $(ABS_$(1))), \
		$(ABS_$(1)))
else
# non-secure boot
REAL_$(1) = $(ABS_$(1))
endif  # ($(CONFIG_ENABLE_SECURITY),)
endef # real_bin

vif     = $(1)_VIF = $(shell echo $(CONFIG_FWPROG_$(1)_VIF) | awk -F "." '{ver = $$1 * 16777216 + $$2 * 65536 + $$3; printf "%s\n", ver}')
fw_flg = FW_$(1)_FLAG = $(if $(findstring .gz,$(REAL_$(1))), 0x04, 0x00)
case_upper = $(shell echo $(1) | tr '[:lower:]' '[:upper:]')
case_lower = $(shell echo $(1) | tr '[:upper:]' '[:lower:]')
qstrip = $(strip $(subst ",,$(1)))

#
# Declare variables
#
all_abs_ibin = $(addprefix ABS_, $(all_PAT))
all_vif = $(addsuffix _VIF, $(all_PAT))
all_fw  = $(addsuffix .fw, $(all_pat))
all_obin = $(addsuffix .bin, $(all_pat))
$(foreach f, $(all_PAT), $(eval $(call abs_bin,$(f))))
$(foreach f, $(all_PAT), $(eval $(call real_bin,$(f))))
$(foreach f, $(all_PAT), $(eval $(call vif,$(f))))

#
# Variables for FIRM_FLAG
#
$(foreach f, $(all_PAT), $(eval $(call fw_flg,$(f))))

#
# .elf targets
#
FWLIST    := $(FWPROG_OUT_DIR)/.fwlist
ifeq ($(FWLIST),$(wildcard $(FWLIST)))
-include $(FWLIST)
FIRMWARES := $(sort $(FIRMWARES))
ifneq ($(CONFIG_ATF_FIP_RTOS),)
FIRMWARES := $(shell echo $(FIRMWARES) | sed -e 's/ sys//g' -e 's/_sys//g')
endif
# History issue: change dsp to sec
FIRMWARES := $(subst sec,dsp,$(FIRMWARES))
endif

FWPROG  = $(addsuffix .elf,$(FIRMWARES))
FWPROG := $(addprefix $(KBUILD_AMBA_FWPROG_OUT_DIR)/,$(FWPROG))

PAYLOAD = $(addprefix $(FWPROG_OUT_DIR)/,$(all_fw))

#
# Compiling flags for FwProg
#
AFLAGS := $(KBUILD_AFLAGS)
CFLAGS_FWPROG := $(KBUILD_CFLAGS)
ifneq ($(CONFIG_LINUX),)
AFLAGS += -static
CFLAGS_FWPROG += -static -Wl,-static
endif

# ---- Definition for link target dependent ---- #
cmd_chk_lib = $(if $(wildcard $(AMBA_O_LIB)/$(1)),$(abspath $(AMBA_O_LIB)/$(1)),$(abspath $(srctree)/$(AMBA_LIB_FOLDER)/$1))

FWPROG_LIB_PATH := -L$(AMBA_O_LIB) -L$(AMBA_LIB_FOLDER)

FWPROG_LIB ?= $(call cmd_chk_lib,libfwprog.a)

FWPROG_LIBS = \
		$(call cmd_chk_lib,libio_common.a)          \
		$(call cmd_chk_lib,libio_$(AMBA_CHIP_ID).a) \
		$(call cmd_chk_lib,libwrap_std.a)           \
		$(FWPROG_LIB)                               \
		$(obj)/libfwprog_user.a

ifneq ($(CONFIG_DEVICE_TREE_SUPPORT),)
FWPROG_LIBS += $(call cmd_chk_lib,libfdt.a)
FWPROG_LIBS += $(call cmd_chk_lib,libwrapc.a)
endif

ifneq ($(AMBA_BSP_NAME),)
FWPROG_LIBS += $(call cmd_chk_lib,libbsp_$(AMBA_BSP_NAME).a)
else
FWPROG_LIBS += $(call cmd_chk_lib,libbsp.a)
endif

ifneq ($(CONFIG_LINUX),)
# Do not use OS-dependent libc
FWPROG_LIBS += $(srctree)/vendors/ambarella/wrapper/std/bin/64b/orig/libc.a.bin
FWPROG_LIBS += $(srctree)/vendors/ambarella/wrapper/std/bin/64b/orig/libm.a.bin
FWPROG_LIBS += $(srctree)/vendors/ambarella/wrapper/std/bin/64b/orig/libgcc.a.bin
CFLAGS_FWPROG += -nolibc -nostdlib
else # RTOS/QNX
ifeq ($(CONFIG_ARM64),y)
FWPROG_C_LIBS = -lc -lm -lgcc
endif
endif # CONFIG_LINUX

LDS_FLAG = -DCONFIG_ARM64
ifeq ($(CONFIG_LINUX),y)
LDS_FLAG += -DCONFIG_LINUX
LNX_LINK = -D__LINK_LNX__
else
LNX_LINK = -D__LINK_LNX__ -D__LINK_DTB__
endif



#
# Top target for all elf
#
$(FWPROG_OUT_DIR)/.stamp_output: fwprog/src $(FWPROG) $(addprefix $(FWPROG_OUT_DIR)/,$(all_obin))
	@touch $@

# Add dependence for %.elf and %-fw
dep_add = $(1): $(2) $(NEWLINE)
fw_fwprog = $(1)-fw := $(2) $(NEWLINE)
$(foreach f,$(FIRMWARES),$(eval $(call dep_add,$(KBUILD_AMBA_FWPROG_OUT_DIR)/$(f).elf,$(FWPROG_OUT_DIR)/AmbaFwProg_$(f).fw $(FWPROG_OUT_DIR)/$(f).lds)))
$(foreach f,$(FIRMWARES),$(eval $(call fw_fwprog,$(KBUILD_AMBA_FWPROG_OUT_DIR)/$(f).elf,$(FWPROG_OUT_DIR)/AmbaFwProg_$(f).fw)))

# Make sure fwprog/src to be built
$(FWPROG_LIBS): fwprog/src

#
# Recipe of %.elf
#
$(FWPROG): $(PAYLOAD) fwprog/src $(FWPROG_LIBS)
	@mkdir -p $(objtree)/$(KBUILD_AMBA_FWPROG_OUT_DIR)
	@echo "  Gen     $@"
	$(Q)$(CC) $(CFLAGS_FWPROG) -Wl,-T,$(FWPROG_OUT_DIR)/$(subst .elf,.lds,$(notdir $@)) \
		-nostartfiles -o $@ $($@-fw) $(FWPROG_LIB_PATH) \
		-Wl,--start-group $(FWPROG_LIBS) -Wl,--end-group \
		$(FWPROG_C_LIBS) \
		-Wl,--gc-sections -Wl,--no-enum-size-warning -Wl,--no-wchar-size-warning -Wl,--build-id=none \
		$(PAYLOAD) \
		-Wl,--cref \
		-Wl,-Map=$(subst dsp,sec,$@).map
	$(Q)$(if $(findstring dsp,$@), mv $@ $(subst dsp,sec,$@))

#
# Recipe of AmbaFwProg_%.fw
#
PRECIOUS += $(FWPROG_OUT_DIR)/AmbaFwProg_%.fw
$(FWPROG_OUT_DIR)/AmbaFwProg_%.fw: $(FWPROG_OUT_DIR)/AmbaFwProgHeader_%.S
	$(Q)mkdir -p $(FWPROG_OUT_DIR)
	$(Q)$(CC) $(AFLAGS) -c $< -o $@

#
# Recipe of AmbaFwProgHeader_%.S
#
PRECIOUS += $(FWPROG_OUT_DIR)/AmbaFwProgHeader_%.S
$(FWPROG_OUT_DIR)/AmbaFwProgHeader_%.S: $(src)/AmbaFwProgHeader.S
	$(Q)mkdir -p $(FWPROG_OUT_DIR)
	$(Q)mydefs=""; \
	$(foreach p,$(all_pat),$(if $(findstring $p,$@),mydefs="$$mydefs -D__LINK_$(call case_upper,$p)__";,)) \
	if [ "x$(CONFIG_LINUX)" = "x" ] && [ "$(findstring lnx,$@)" != "" ]; then \
		mydefs="$$mydefs -D__LINK_DTB__"; \
	fi; \
	$(if $(Q),,echo "mydefs: $$mydefs";) \
	echo $(CPP) $(CFLAGS_FWPROG) -D__ASM__ $$mydefs $< | sed /^#/d > $@; \
	$(CPP) $(CFLAGS_FWPROG) -D__ASM__ $$mydefs $< | sed /^#/d > $@

#
# Recipe of $(FWPROG_OUT_DIR)/%.lds (not exist %.lds rule)
#
PRECIOUS += $(FWPROG_OUT_DIR)/%.lds
$(FWPROG_OUT_DIR)/%.lds: $(src)/amba_fwprog.lds.S
	$(Q)mkdir -p $(FWPROG_OUT_DIR)
	$(Q)mydefs="-DCONFIG_FWPROG_SYS_LOADADDR=$(CONFIG_FWPROG_SYS_LOADADDR)"; \
	$(foreach p,$(all_pat),$(if $(findstring $p,$@),mydefs="$$mydefs -D__LINK_$(call case_upper,$p)__";,)) \
	if [ "x$(CONFIG_LINUX)" = "x" ] && [ "$(findstring lnx,$@)" != "" ]; then \
		mydefs="$$mydefs -D__LINK_DTB__"; \
	fi; \
	$(if $(Q),,echo "mydefs: $$mydefs";) \
	echo $(CPP) $(CFLAGS_FWPROG) -DFW_OUT_DIR=$(FWPROG_OUT_DIR) -D__ASM__ $$mydefs $(LDS_FLAG) $< | sed /^#/d > $@; \
	$(CPP) $(CFLAGS_FWPROG) -DFW_OUT_DIR=$(FWPROG_OUT_DIR) $$mydefs $(LDS_FLAG) $< | sed /^#/d > $@

# ------------------------------------------------------------------ #
#
# Firmware object files
#

ifneq ($(CONFIG_ENABLE_SECURITY),)
#------------------------------------------------------------------------------
#
# Signed binary targets
# TODO: automatically expand targets by config
#

# Targets to be expanded for RSA
define sign_img_amba_rsa
$(FWPROG_OUT_DIR)/$(1).signed: $(ABS_$(1))
	$(Q)mkdir -p $(FWPROG_OUT_DIR)
	$(Q)[ -s $$< ] && cp $$< $$@.in || (echo "$$< NOT exist" && exit 1)
	$(Q)$(LINK_SH_secure_tool) securebin_gen \
		-k $(CONFIG_RSA_PRIVATE_KEY_PATH) \
		-i $$@.in \
		-o $$@ \
		-a align_256 \
		-t rsa \
		-p y \
		$(if $(call qstrip,$(CONFIG_OPENSSL_PATH)),-e $(CONFIG_OPENSSL_PATH),)

endef # sign_img_amba_rsa

# Targets to be expanded for ECC
define sign_img_amba_ecc
$(FWPROG_OUT_DIR)/$(1).signed: $(ABS_$(1))
ifneq ("$(1)","BST")
	$(Q)mkdir -p $(FWPROG_OUT_DIR)
	$(Q)[ -s $$< ] && cp $$< $$@.in || (echo "$$< NOT exist" && exit 1)
	$(Q)$(LINK_SH_secure_tool) securebin_gen \
		-k $(CONFIG_ECC_PRIVATE_KEY_PATH) \
		-K $(CONFIG_SECURE_ECC_IDX) \
		-i $$@.in \
		-o $$@ \
		-a align_256 \
		-t ecc \
		-p y \
		$(if $(call qstrip,$(CONFIG_OPENSSL_PATH)),-e $(CONFIG_OPENSSL_PATH),)

else
ifneq ($(CONFIG_BST_LARGE_SIZE),)
ifeq ($(CONFIG_SOC_CV5)$(CONFIG_SOC_CV52),y)
#Skip the step of LINK_SH_secure_tool for amba_bst.bin
## +-------------------------+
## | amba_bst.bin (7168B)    |
## +-------------------------+
## | amba_bst_bin.temp (960B)|
## +-------------------------+
## | amba_bst_bin.sign (64B) |(amba_bst_bin.sign = sign(amba_bst.bin))
## +-------------------------+
## | amba_bst_bin.apd (8128B)|
## +-------------------------+
## | amba_bst_bin.sign (64B) |(amba_bst_bin.sign = sign(amba_bst_bin.apd))
## +-------------------------+
	@tail -c +8193 $(ABS_$(1)) > $(ABS_$(1)).apd
	@truncate -s 8128 $(ABS_$(1))
	@python ${srctree}/tools/hostutils/ust_header_modify.py $(ABS_$(1)) 0 0 7168
	@tail -c +7169 $(ABS_$(1)) > $(ABS_$(1)).temp
	@truncate -s 7168 $(ABS_$(1))
	@${CONFIG_OPENSSL_PATH} pkeyutl -sign -inkey ${CONFIG_ECC_PRIVATE_KEY_PATH} -out $(ABS_$(1)).sign -rawin -in $(ABS_$(1))
	@cat $(ABS_$(1)).temp >> $(ABS_$(1))
	@cat $(ABS_$(1)).sign >> $(ABS_$(1))
	@truncate -s 8128 $(ABS_$(1)).apd
	@python ${srctree}/tools/hostutils/ust_header_modify.py $(ABS_$(1)).apd 0 0 0
	@${CONFIG_OPENSSL_PATH} pkeyutl -sign -inkey ${CONFIG_ECC_PRIVATE_KEY_PATH} -out $(ABS_$(1)).sign -rawin -in $(ABS_$(1)).apd
	@cat $(ABS_$(1)).apd >> $(ABS_$(1))
	@cat $(ABS_$(1)).sign >> $(ABS_$(1))
	@rm $(ABS_$(1)).*
	@cp "$(ABS_$(1))" "$(FWPROG_OUT_DIR)/$(1).signed"
else
	$(Q)mkdir -p $(FWPROG_OUT_DIR)
	$(Q)[ -s $$< ] && cp $$< $$@.in || (echo "$$< NOT exist" && exit 1)
	$(Q)$(LINK_SH_secure_tool) securebin_gen \
		-k $(CONFIG_ECC_PRIVATE_KEY_PATH) \
		-K $(CONFIG_SECURE_ECC_IDX) \
		-i $$@.in \
		-o $$@ \
		-a align_256 \
		-t ecc \
		-p y \
		$(if $(call qstrip,$(CONFIG_OPENSSL_PATH)),-e $(CONFIG_OPENSSL_PATH),)
endif
else
	$(Q)mkdir -p $(FWPROG_OUT_DIR)
	$(Q)[ -s $$< ] && cp $$< $$@.in || (echo "$$< NOT exist" && exit 1)
	$(Q)$(LINK_SH_secure_tool) securebin_gen \
		-k $(CONFIG_ECC_PRIVATE_KEY_PATH) \
		-K $(CONFIG_SECURE_ECC_IDX) \
		-i $$@.in \
		-o $$@ \
		-a align_256 \
		-t ecc \
		-p y \
		$(if $(call qstrip,$(CONFIG_OPENSSL_PATH)),-e $(CONFIG_OPENSSL_PATH),)
endif
endif
endef # sign_img_amba_ecc

IMG_BOOT_SIGN := BST BLD BLD2
ifneq ($(CONFIG_SECURE_TYPE_RSA),)
$(foreach f, $(IMG_BOOT_SIGN), $(if $(findstring signed,$(REAL_$f)), $(eval $(call sign_img_amba_rsa,$(f)))))
else # ECC
$(foreach f, $(IMG_BOOT_SIGN), $(if $(findstring signed,$(REAL_$f)), $(eval $(call sign_img_amba_ecc,$(f)))))
endif # $(CONFIG_SECURE_TYPE)

ifneq ($(CONFIG_LINUX),)
IMG_APP_SIGN += LNX
else
IMG_APP_SIGN += SYS
endif
IMG_APP_SIGN += ATF
IMG_APP_SIGN += R52SYS
IMG_APP_SIGN += XTB
$(foreach f, $(IMG_APP_SIGN), $(if $(findstring signed,$(REAL_$f)), $(eval $(call sign_img_amba_rsa,$(f)))))

endif # neq ($(CONFIG_ENABLE_SECURITY),)

#------------------------------------------------------------------------------
#
# Macro to build .fw
#
ifeq ($(CONFIG_ARM64),y)
OUTPUT_FORMAT = elf64-littleaarch64
endif

#ifndef DATE
#DATE        :=  $(shell date +%Y%m%d)
#endif
YEAR        :=  $(shell date +%Y | sed s/^0//)
MONTH       :=  $(shell date +%m | sed s/^0//)
DAY         :=  $(shell date +%d | sed s/^0//)
MY_DATE     :=  $(shell echo $(YEAR)\*65536\+$(MONTH)\*256\+$(DAY) | bc)


# $1 is uppercase
cmd_cc_fw_full = \
	$(CC) $(AFLAGS) \
		-DCRC32=$$(word 3,$$(shell $(AMBA_HOST_CRC) $(abspath $(REAL_$1)))) \
		-DIMG_LEN=$$(word 2,$$(shell $(AMBA_HOST_CRC) $(abspath $(REAL_$1)))) \
		-DFIRM_VER_NUM=$($1_VIF) \
		-DFIRM_VER_DATE=$(MY_DATE) \
		$(if $(CONFIG_FWPROG_$1_LOADADDR),-DFIRM_MEM_ADDR=$(CONFIG_FWPROG_$1_LOADADDR)) \
		-DPARTHD_MAGIC=0xa324eb90 \
		-DINCBIN=$(abspath $(REAL_$1)) \
		$(if $(wildcard $(call case_lower,$1).tmp),-DOIMG_LEN=$(word 2,$(shell $(AMBA_HOST_CRC) $(call case_lower,$1).tmp))) \
		-DFIRM_FLAG=$(FW_$1_FLAG) \
		-c $(srctree)/$(src)/AmbaFwImgHeader.S -o $$@.elf

cmd_cc_fw_simple = \
	$(CC) $(AFLAGS) \
		-c $(srctree)/$(src)/AmbaFwImgHeader.S -o $$@.elf

#------------------------------------------------------------------------------
# .fw rules
define def_fw
$(FWPROG_OUT_DIR)/$(call case_lower,$1).fw: $(REAL_$1) $(src)/AmbaFwImgHeader.S
	$(Q)mkdir -p $(FWPROG_OUT_DIR)
	$(Q)$(if $(REAL_$1),$(call cmd_cc_fw_full,$1),$(call cmd_cc_fw_simple))
	$(Q)$(OBJCOPY) -j .text -O $(OUTPUT_FORMAT) $$@.elf $$@
	@rm -f $$@.elf

endef
$(foreach f, $(all_PAT), $(eval $(call def_fw,$(f))))

#------------------------------------------------------------------------------
# .bin rules
$(FWPROG_OUT_DIR)/%.bin: $(FWPROG_OUT_DIR)/%.fw
	$(Q)$(OBJCOPY) -O binary -R .comment -R .note -S $< $@


#------------------------------------------------------------------------------
# special .bin rules

ifneq ($(REAL_DSP),)
DSP_LIST := $(abspath $(shell echo $(CONFIG_FWPROG_DSP_LIST_FILE)))
$(REAL_DSP): FORCE
	@echo "  Gen     $@"
	$(Q)cd $(srctree) && $(AMBA_HOST_ROMFS) $(DSP_LIST) $@ 0x1000000 && cd -
	$(Q)cp $@ $@.tmp

endif

ifneq ($(REAL_ROM),)
ROM_LIST := $(abspath $(shell echo $(CONFIG_FWPROG_ROM_LIST_FILE)))
$(REAL_ROM): FORCE
	@echo "  Gen     $@"
	$(Q)cd $(srctree) && $(AMBA_HOST_ROMFS) $(ROM_LIST) $@ 0x2000000 && cd -
	$(Q)cp $@ $@.tmp

endif

#------------------------------------------------------------------------------

ifneq ($(KBUILD_VERBOSE),0)
# Keep intermediate targes for debugging,
# or using ".SECONDARY" with no prerequisites causes all targets to be treated as secondary
# (i.e., no target is removed because it is considered intermediate).
# http://www.gnu.org/software/make/manual/html_node/Special-Targets.html
#.SECONDARY:
.PRECIOUS: $(PRECIOUS)
endif # $(KBUILD_VERBOSE)

# ------------------------------------------------------------------ #
clean-dirs  := $(DIR_amba_fwprog_out)

