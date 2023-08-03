
obj-$(CONFIG_LIBMETAL) += .stamp_build

ifneq ($(CONFIG_LIBMETAL),)

LIBMETAL_SRC = libmetal-2022.04.0

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

KBUILD_CFLAGS += -Wno-strict-prototypes
KBUILD_CFLAGS += -Wno-pointer-arith


$(obj)/.stamp_conf: $(KCONFIG)
	$(Q)mkdir -p $(obj)/build
	$(Q)cd $(obj)/build && cmake \
		$(srctree)/$(src)/$(LIBMETAL_SRC) \
		-G "Unix Makefiles" \
		$(if $(CROSS_COMPILE),-DCROSS_PREFIX=$(CROSS_COMPILE)) \
		-DCMAKE_TOOLCHAIN_FILE=ambarella-generic \
		-DCMAKE_C_FLAGS="$(KBUILD_CFLAGS)" \
		-DWITH_DEFAULT_LOGGER=0
	@touch $@

#
# Note:
# $(srctree)/$(src)/include have a copy of auto-generated headers for parallel building.
# If here is any update for libmetal headers,
# *NEED* to rebuild libmetal first.
#
$(obj)/.stamp_build: $(obj)/.stamp_conf FORCE
	$(Q)cd $(obj)/build && $(MAKE) \
		$(CMAKE_VERBOSE) && \
		rsync -au ./lib/include/ $(srctree)/$(src)/include
	@touch $@

$(obj)/build/lib/libmetal.a: $(obj)/.stamp_build
	@:

install-files := build/lib/libmetal.a
clean-files += .stamp_conf .stamp_build
clean-dirs  += build

endif # ($(CONFIG_LIBMETAL),)

