ifeq ($(CONFIG_ENABLE_AMBALINK),y)

#KBUILD_CFLAGS := $(subst -O1,-O0,$(KBUILD_CFLAGS))
#KBUILD_CFLAGS := $(subst -O2,-O0,$(KBUILD_CFLAGS))

ifneq ($(CONFIG_OPENAMP),)

obj-y += AmbaOpenAmp.o

OBJ_OPENAMP := \
		amba_metal.o		\
		amba_remoteproc.o	\
		amba_platform_info.o	\
		amba_rsc_table.o

# TODO: Master instance for openamp
#ifneq ($(CONFIG_AMBALINK_RPMSG_ROLE_MAST),)
#OBJ_OPENAMP += master.o
#else
OBJ_OPENAMP += slave.o
#endif # Master for openamp
AmbaOpenAmp-objs += $(foreach o,$(OBJ_OPENAMP),$(o))

ifeq ($(CONFIG_CC_USESTD),)
AMBA_OPENAMP_CFLAGS := -include $(srctree)/vendors/foss/libwrapc/AmbaStdWrap.h
AMBA_OPENAMP_CFLAGS += -I$(srctree)/vendors/foss/libwrapc/inc_wrap
endif
AMBA_OPENAMP_CFLAGS += \
	-I$(srctree)/vendors/foss/libmetal/include		\
	-I$(srctree)/vendors/foss/open-amp/src/lib/include	\
	-I$(srctree)/$(src)

DECLAR_CFLAGS = $1 := $(AMBA_OPENAMP_CFLAGS)
$(foreach o,$(OBJ_OPENAMP),$(eval $(call DECLAR_CFLAGS,CFLAGS_$(o))))
endif

endif # $(CONFIG_ENABLE_AMBALINK)
