
-include $(objtree)/include/config/auto.conf

ifneq ($(CONFIG_ATF_SPD_OPTEE),)
CONFIG_TRUSTZONE_OPTEE := y
else
CONFIG_TRUSTZONE_OPTEE := n
endif
CONFIG_TRUSTZONE_USING_RPC_TOOLKIT := n


ifneq ($(CONFIG_ATF_BUILD),)
CERT_BIN_PATH := $(objtree)/$(obj)/cert/bin
else
CERT_BIN_PATH := $(CURDIR)/bin
endif
CERT_CREATE := $(CERT_BIN_PATH)/cert_create
FIPTOOL := $(CERT_BIN_PATH)/fiptool

ATF_BL31 := $(shell echo $(CONFIG_ATF_BL31_BIN))
ATF_BL32 := $(shell echo $(CONFIG_ATF_BL32_BIN))
ATF_BL33 := $(shell echo $(CONFIG_ATF_BL33_BIN))
#ATF_BL33 := /dump11/chho/SH/ambarella/out/cv22bub/amboot/bld_debug.bin

ifdef CONFIG_ATF_FIP_RTOS
KBUILD_AMBA_OUT_DIR := ${O}/out
ATF_SCP  := $(shell echo $(CONFIG_FWPROG_SYS))
endif

FIP_BIN         := fip.bin

FIP_OPTIONS     := --trusted-key-cert crt/trusted_key.crt
FIP_OPTIONS     += --soc-fw-cert crt/soc_fw_content.crt
FIP_OPTIONS     += --soc-fw-key-cert crt/soc_fw_key.crt
FIP_OPTIONS     += --nt-fw-cert crt/nt_fw_content.crt
FIP_OPTIONS     += --nt-fw-key-cert crt/nt_fw_key.crt
FIP_OPTIONS     += --soc-fw $(ATF_BL31) --nt-fw $(ATF_BL33)
ifeq ($(CONFIG_TRUSTZONE_OPTEE), y)
FIP_OPTIONS     += --tos-fw-cert crt/tos_fw_content.crt
FIP_OPTIONS     += --tos-fw-key-cert crt/tos_fw_key.crt
FIP_OPTIONS     += --tos-fw $(ATF_BL32)
endif
ifdef CONFIG_ATF_FIP_RTOS
FIP_OPTIONS	+= --scp-fw-cert     crt/scp_fw_content.crt
FIP_OPTIONS	+= --scp-fw-key-cert crt/scp_fw_key.crt
FIP_OPTIONS	+= --scp-fw $(ATF_SCP)
endif

CERT_OPTIONS    := -n --tfw-nvctr 0 --ntfw-nvctr 0 --key-alg rsa
ifeq ($(CONFIG_ENABLE_SECURITY), y)
CERT_OPTIONS    += --rot-key $(CONFIG_RSA_PRIVATE_KEY_PATH)
endif
#ifeq ($(CONFIG_TRUSTZONE_USING_RPC_TOOLKIT), y)
#CERT_OPTIONS    += --localport $(CONFIG_TRUSTZONE_RPC_TOOLKIT_LOCAL_PORT)
#else
#CERT_OPTIONS    += --rot-key $(TOP)/build/keys/rot_private.pem
#endif


$(FIP_BIN): $(ATF_BL31) $(ATF_BL32) $(ATF_BL33)
	$(Q)mkdir -p crt
	$(Q)$(CERT_CREATE) $(CERT_OPTIONS) $(FIP_OPTIONS)
	$(Q)$(FIPTOOL) create --align 32 $(FIP_OPTIONS) $@
	$(Q)cp -fv $@ ${objtree}/out

