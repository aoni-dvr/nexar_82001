################################################################################
#             Common
################################################################################
include $(srctree)/$(CV_CORE_DIR)/arm_framework/cvchip.mk

ifeq ($(CONFIG_SOC_CV1),y)
ifneq ($(CVCHIP),CHIP_CV1)
$(error PROJECT is not cv1 !!!)
endif

else ifeq ($(CONFIG_SOC_CV2),y)
ifneq ($(CVCHIP),CHIP_CV2)
$(error PROJECT is not cv2 !!!)
endif

else ifeq ($(CONFIG_SOC_CV22),y)
ifneq ($(CVCHIP),CHIP_CV22)
$(error PROJECT is not cv2s or cv22 !!!)
endif

else ifeq ($(CONFIG_SOC_CV25),y)
ifneq ($(CVCHIP),CHIP_CV25)
$(error PROJECT is not cv25 !!!)
endif

else ifeq ($(CONFIG_SOC_CV28),y)
ifneq ($(CVCHIP),CHIP_CV28)
$(error PROJECT is not cv28 !!!)
endif

else ifeq ($(CONFIG_SOC_CV2FS),y)
ifeq ($(CVCHIP),CHIP_CV2FS)
else ifeq ($(CVCHIP),CHIP_CV22FS)
else ifeq ($(CVCHIP),CHIP_CV2A)
else ifeq ($(CVCHIP),CHIP_CV22A)
else
$(error PROJECT is not cv2fs or cv22fs !!!)
endif

else ifeq ($(CONFIG_SOC_CV5)$(CONFIG_SOC_CV52),y)
ifneq ($(CVCHIP),CHIP_CV5)
ifneq ($(CVCHIP),CHIP_CV52)
$(error PROJECT is not cv5/cv52 !!!)
endif
endif
endif

ccflags-y += -I$(srctree)/vendors/ambarella/inc                                 \
	     -I$(srctree)/soc/osal                                      			\
	     -I$(srctree)/soc/io/src/common/inc                                      \
	     -I$(srctree)/$(CV_CORE_DIR)/cv_common/inc                                      \
	     -I$(srctree)/$(CV_CORE_DIR)/cv_common/inc/cvapi                                \
	     -I$(srctree)/$(CV_CORE_DIR)/cv_common/inc/cavalry                               \
	     -I$(srctree)/$(CV_CORE_DIR)/cv_common/inc/cvsched_common                        \
	     -D$(CVCHIP)                                                        \
	     -O3 -g -DAMBA_KAL_SOURCE_CODE

ifeq ($(CVCHIP),CHIP_CV2A)
ccflags-y += -DASIL_COMPLIANCE
ifeq ($(CONFIG_AMBALINK_RPMSG_G2),y)
ifeq ($(CONFIG_ENABLE_CV_MONITOR),y)
ccflags-y += -DASIL_SAFETY_MONITOR
endif
endif

else ifeq ($(CVCHIP),CHIP_CV22A)
ccflags-y += -DASIL_COMPLIANCE
ifeq ($(CONFIG_AMBALINK_RPMSG_G2),y)
ifeq ($(CONFIG_ENABLE_CV_MONITOR),y)
ccflags-y += -DASIL_SAFETY_MONITOR
endif
endif

else ifeq ($(CVCHIP),CHIP_CV2FS)
ccflags-y += -DASIL_COMPLIANCE
ifeq ($(CONFIG_AMBALINK_RPMSG_G2),y)
ifeq ($(CONFIG_ENABLE_CV_MONITOR),y)
ccflags-y += -DASIL_SAFETY_MONITOR
endif
endif

else ifeq ($(CVCHIP),CHIP_CV22FS)
ccflags-y += -DASIL_COMPLIANCE
ifeq ($(CONFIG_AMBALINK_RPMSG_G2),y)
ifeq ($(CONFIG_ENABLE_CV_MONITOR),y)
ccflags-y += -DASIL_SAFETY_MONITOR
endif
endif
endif

ifeq ($(CONFIG_DISABLE_ARM_CVTASK),y)
ccflags-y += -DDISABLE_ARM_CVTASK
endif

ifeq ($(CONFIG_DISABLE_CAVALRY),y)
ccflags-y += -DDISABLE_CAVALRY
endif

ifeq ($(CONFIG_BUILD_MAL),y)
ccflags-y += -DENABLE_AMBA_MAL
endif

CVDIR := $(srctree)/$(src)/arm_framework

ifneq ("$(wildcard $(srctree)/$(CV_CORE_DIR)/cv_common/lib_root/local_arm_lib_root)","")
CV_RELEASE_DIR=$(AMBA_LIB_FOLDER)/$(AMBA_CHIP_ID)
else
CV_RELEASE_DIR=$(AMBA_LIB_FOLDER)/$(AMBA_CHIP_ID)
endif

################################################################################
#             build lib for each cvapp
################################################################################
define ADD_CVAPP_LIB_TARGET
ifneq ($(wildcard $(CVDIR)/app/$(1)),)
# With cvapp source
$(1)_NAME = $$(notdir $(1))
obj-y += arm_framework/app/$(1)/
else
# No cvapp source
obj-y += install-cvapp-$(1)-as-bin
APP_$(1)_LIBS = $$(patsubst %, $$(CV_RELEASE_DIR)/libcv_%.a, $(1))
$(obj)/install-cvapp-$(1)-as-bin:
	@mkdir -p $$(AMBA_O_LIB)
	install -m 644 $$(APP_$(1)_LIBS) $$(AMBA_O_LIB)
endif
endef
$(foreach m,$(shell echo $(CVAPP_AS_SRC)),$(eval $(call ADD_CVAPP_LIB_TARGET,$m)))

################################################################################
#             build lib/tbar for each cvtask
################################################################################
define ADD_LIB_TARGET
ifneq ($(wildcard $(CVDIR)/cvtask/$(1)),)
# With cvtask source
$(1)_NAME = $$(notdir $(1))
obj-y += arm_framework/cvtask/$(1)/
$(1)_MNFT = $$(wildcard $(CVDIR)/cvtask/$(1)/*.mnft)
TBARS +=  $$(patsubst $(srctree)/%mnft,$(objtree)/%tbar,$$($(1)_MNFT))
else
# No cvtask source
obj-y += install-$(1)-as-bin
CVTASK_$(1)_LIBS = $$(patsubst %, $$(CV_RELEASE_DIR)/libcv_%.a, $(1))
$(obj)/install-$(1)-as-bin:
	@mkdir -p $$(AMBA_O_LIB)
	install -m 644 $$(CVTASK_$(1)_LIBS) $$(AMBA_O_LIB)
endif
endef

$(foreach m,$(shell echo $(CVTASK_AS_SRC)),$(eval $(call ADD_LIB_TARGET,$m)))

# Generate cvtasks_lib_list file used by linking stage
.PHONY: cvtask_libs

obj-y += cvtask_libs
$(obj)/cvtask_libs: FORCE
	$(Q)echo CVTASK_AS_SRC=\"$(CVTASK_AS_SRC)\" > $(objtree)/$(obj)/cvtask_libs
	$(Q)echo CVTASK_AS_BIN=\"$(CVTASK_AS_BIN)\" >> $(objtree)/$(obj)/cvtask_libs
	$(Q)echo CVAPP_AS_SRC=\"$(CVAPP_AS_SRC)\" >> $(objtree)/$(obj)/cvtask_libs
	$(Q)echo CVAPP_AS_BIN=\"$(CVAPP_AS_BIN)\" >> $(objtree)/$(obj)/cvtask_libs
	@echo "Gen $(obj)/cvtask_libs"

TBARDIR = $(objtree)/$(obj)/cvtask_tbar
$(objtree)/%tbar: $(srctree)/%mnft
	@echo "  TABLE_AR     $*tbar"
	$(Q)$(TABLE_AR) -c $@ -a $<
	$(Q)mkdir -p $(TBARDIR); install -t $(TBARDIR) $@

################################################################################
#             build app-specific tbar files
################################################################################
TABLE_AR = table_ar

obj-y += $(patsubst %.mnft,%.tbar,$(shell echo $(APP_MNFT_FILES)))

$(obj)/%.tbar: $(srctree)/$(src)/config/%.mnft $(TBARS)
	@echo "  TABLE_AR     $@"
	$(Q)$(TABLE_AR) -c $@ -a $<
	$(Q)test -z $(strip $(TBARS)) || $(TABLE_AR) $@ -cat $(TBARS)

################################################################################
#             build app-specific sfb files
################################################################################
SFC = sfc

obj-y += $(patsubst %.csv,%.sfb,$(shell echo $(APP_CSV_FILES)))

$(obj)/%.sfb: $(srctree)/$(src)/config/%.csv
	@echo "  SFC     $@"
	$(Q)$(SFC) -o $@ $<
