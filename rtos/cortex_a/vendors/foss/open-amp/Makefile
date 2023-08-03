
obj-$(CONFIG_OPENAMP) += .stamp_build

ifneq ($(CONFIG_OPENAMP),)

OPENAMP_SRC = open-amp-2022.04.0

ifeq ($(KBUILD_VERBOSE),1)
CMAKE_VERBOSE = VERBOSE=1
endif

# Debug
#KBUILD_CFLAGS := $(subst -O1,-O0,$(KBUILD_CFLAGS))
#KBUILD_CFLAGS := $(subst -O2,-O0,$(KBUILD_CFLAGS))

ifeq ($(CONFIG_CC_USESTD),)
NOSTDINC_FLAGS += -nostdinc -isystem $(shell $(CC) -print-file-name=include)
KBUILD_CFLAGS += $(NOSTDINC_FLAGS)
KBUILD_CFLAGS += -include $(srctree)/vendors/foss/libwrapc/AmbaStdWrap.h
KBUILD_CFLAGS += -I$(srctree)/vendors/foss/libwrapc/inc_wrap
endif
KBUILD_CFLAGS += -include $(objtree)/include/generated/autoconf.h
ifeq ($(CONFIG_THREADX),y)
KBUILD_CFLAGS += -I$(srctree)/vendors/ambarella/inc
endif

KBUILD_CFLAGS += -Wno-stringop-truncation
KBUILD_CFLAGS += -Wno-pedantic

KBUILD_CFLAGS += -I$(srctree)/vendors/foss/libmetal/include
KBUILD_CFLAGS += -DRPMSG_BUFFER_SIZE=CONFIG_AMBALINK_RPMSG_SIZE

$(obj)/.stamp_conf: $(KCONFIG)
	@mkdir -p $(obj)/build
	@cd $(obj)/build && cmake \
		$(srctree)/$(src)/$(OPENAMP_SRC) \
		-G "Unix Makefiles" \
		$(if $(CROSS_COMPILE),-DCROSS_PREFIX=$(CROSS_COMPILE)) \
		-DCMAKE_TOOLCHAIN_FILE=ambarella-generic \
		-DCMAKE_C_FLAGS="$(KBUILD_CFLAGS)" \
		-DWITH_LIBMETAL_FIND=0
	@touch $@

$(obj)/.stamp_build: $(obj)/.stamp_conf FORCE
	@cd $(obj)/build && $(MAKE) \
		$(CMAKE_VERBOSE)
	@touch $@

$(obj)/build/lib/libopen_amp.a: $(obj)/.stamp_build
	@:

install-files := build/lib/libopen_amp.a
clean-files += .stamp_conf .stamp_build
clean-dirs  += build

endif # ($(CONFIG_OPENAMP),)

