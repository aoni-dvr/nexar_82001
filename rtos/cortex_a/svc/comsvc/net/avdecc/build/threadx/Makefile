
# Need to disable flags to use headers of toolchain.
NOSTDINC_FLAGS :=

obj-y += libnet_avdecc.a

libnet_avdecc-objs := \
	../../AvbSvc.o

libnet_avdecc-objs += \
	../../util.o	\
	../../avdecc.o	\
	../../adp.o	\
	../../acmp.o	\
	../../aecp.o	\
	../../talker.o	\
	../../listener.o	\
	../../controller.o


# Dependency of AmbaVer.o
$(obj)/AmbaVer.o: $(call amba_ver_dep,AmbaVer.o,$(libnet_avdecc-objs))

CFLAGS_AmbaVer.o = -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)		\
		   -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM)	\
		   -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)	\
		   -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)	\
		   -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)		\
		   -DAMBA_CI_ID=$(AMBA_CI_ID)

ccflags-y += -Ivendors/ambarella/inc	\
			-Ivendors/ambarella/inc/io	\
			-Isvc/comsvc/print	\
			-Isvc/comsvc/misc	\
			-Isvc/comsvc/net/inc	\
			-Isvc/comsvc/net/avb/avtp	\
			-I$(srctree)/$(src)/../../inc	\
			-I$(srctree)/$(src)/../..

install-files := libnet_avdecc.a


