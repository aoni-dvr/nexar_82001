ifeq ($(CONFIG_ENABLE_AMBALINK),y)

#KBUILD_CFLAGS := $(subst -O1,-O0,$(KBUILD_CFLAGS))
#KBUILD_CFLAGS := $(subst -O2,-O0,$(KBUILD_CFLAGS))

obj-y += link_user.o

# Linkuser module
link_user-objs := AmbaLink_AmbaShell.o	\
		  AmbaLinkMemoryCfg.o

ifneq ($(CONFIG_AMBALINK_BOOT_OS),)
link_user-objs += \
		  AmbaHiber_Test.o	\
		  AmbaLinkCvShmCfg.o	\
		  AmbaIPC_RpcProg_Util.o

ccflags-y += -I$(srctree)/vendors/foss/libwrapc
endif # CONFIG_AMBALINK_BOOT_OS

#ifeq ($(CONFIG_CPU_CORTEX_R52),)
#ccflags-y += \
#        -I$(srctree)/vendors/ambarella/inc/cvflow                      \
#        -I$(srctree)/soc/cvflow/cv_common/inc                          \
#        -I$(srctree)/soc/cvflow/cv_common/inc/cvapi                    \
#        -I$(srctree)/svc/comsvc/cv/cv_common/inc/cvapi
#endif

ifeq ($(CONFIG_ICAM_ENABLE_REGRESSION_FLOW),y)
link_user-objs += AmbaIPC_Stream.o
link_user-objs += AmbaIPC_Regression.o

CFLAGS__Regression.o = -I$(srctree)/svc/comsvc/codec/lib/src/common/dataflow/container/inc
CFLAGS_AmbaIPC_Stream.o = -I$(srctree)/svc/comsvc/cfs/inc \
			  -I$(srctree)/svc/comsvc/codec/lib/src/common/dataflow/container/inc
endif


endif # $(CONFIG_ENABLE_AMBALINK)

