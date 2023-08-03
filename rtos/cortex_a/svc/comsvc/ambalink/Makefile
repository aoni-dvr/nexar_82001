
ifeq ($(CONFIG_BUILD_HOST_CRC),y)
subdir-y += tools/hostutils
endif

ifeq ($(CONFIG_ENABLE_AMBALINK),y)
ifeq ($(CONFIG_THREADX),y)

#KBUILD_CFLAGS := $(subst -O1,-O0,$(KBUILD_CFLAGS))
#KBUILD_CFLAGS := $(subst -O2,-O0,$(KBUILD_CFLAGS))

# TODO: fix compiling error under inc/rpcprog/
KBUILD_CFLAGS += -Wno-pedantic


link_subdirs := core
link_subdirs += link_user
link_subdirs += app
ifneq ($(CONFIG_OPENAMP),)
link_subdirs += AmbaOpenAmp
endif

#subdir-y += $(link_subdirs)

link_subobjs := $(foreach d,$(link_subdirs),$(d)/$(d).o)
link_subobjs_dirs := $(foreach d,$(link_subdirs),$(src)/$(d))

obj-y += libambalink.a
libambalink-objs := $(link_subobjs)

libambalink-objs += AmbaVer.o
$(obj)/AmbaVer.o :  $(call amba_ver_dep,AmbaVer.o,$(libambalink-objs))
CFLAGS_AmbaVer.o =      -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)            \
                        -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM)    \
                        -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)      \
                        -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)      \
                        -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)                \
                        -DAMBA_CI_ID=$(AMBA_CI_ID)

$(obj)/AmbaVer.o $(src)/libambalink.a: $(link_subobjs_dirs) FORCE

$(link_subobjs_dirs): FORCE
	$(Q)$(MAKE) $(build)=$@

# sub-objs needs to depends on subdir
DECLAR_DEP = $(src)/$(1)/$(1).o: $(src)/$(1)
$(foreach o,$(link_subdirs),$(eval $(call DECLAR_DEP,$(o))))

# Depends on sub-objs
libambalink.a: $(link_subobjs)

IO_FOLDER = soc/io/src

subdir-ccflags-y += \
	-I$(srctree)/vendors/ambarella/inc \
	-I$(srctree)/vendors/ambarella/inc/io \
	-I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \

subdir-ccflags-y += \
	-I$(srctree)/$(IO_FOLDER)/common/inc \
	-I$(srctree)/$(IO_FOLDER)/common/inc/arm  \
	-I$(srctree)/$(IO_FOLDER)/common/inc/csl  \
	-I$(srctree)/$(IO_FOLDER)/common/inc/reg  \
	-I$(srctree)/$(IO_FOLDER)/common/inc/rtsl  \
	-I$(srctree)/$(IO_FOLDER)/$(AMBA_CHIP_ID)/inc \
	-I$(srctree)/$(IO_FOLDER)/$(AMBA_CHIP_ID)/inc/csl \
	-I$(srctree)/$(IO_FOLDER)/$(AMBA_CHIP_ID)/inc/reg \
	-I$(srctree)/$(IO_FOLDER)/$(AMBA_CHIP_ID)/inc/rtsl \
	-I$(srctree)/svc/comsvc/print \
	-I$(srctree)/svc/comsvc/misc \

subdir-ccflags-y += \
	-I$(srctree)/$(src)/inc \
	-I$(srctree)/$(src)/inc/rpcprog \
	-I$(srctree)/$(src)/core/inc \
	-I$(srctree)/svc/comsvc/shell/inc

# TODO: by config and need to sync with Linux
subdir-ccflags-y += -DRPC_DEBUG

subdir-asflags-y += \
	-I$(srctree)/$(IO_FOLDER)/$(AMBA_CHIP_ID)/inc \
	-I$(srctree)/vendors/ambarella/inc


install-files := libambalink.a
endif # $(CONFIG_ENABLE_AMBALINK)
endif # $(CONFIG_THREADX)

