
# Need to disable flags to use headers of toolchain.
NOSTDINC_FLAGS :=

obj-y += libcomsvc_fifo.a

libcomsvc_fifo-objs := \
	../../SvcFIFO.o

# Dependency of AmbaVer.o
$(obj)/AmbaVer.o: $(call amba_ver_dep,AmbaVer.o,$(libcomsvc_fifo-objs))

CFLAGS_AmbaVer.o = -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)		\
		   -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM)	\
		   -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)	\
		   -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)	\
		   -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)		\
		   -DAMBA_CI_ID=$(AMBA_CI_ID)

ccflags-y += -Ivendors/ambarella/inc	\
			-Ivendors/ambarella/inc/io	\
			-Isvc/comsvc/print \
			-I$(srctree)/$(src)/../../inc	\
			-I$(srctree)/$(src)/../..

install-files := libcomsvc_fifo.a


