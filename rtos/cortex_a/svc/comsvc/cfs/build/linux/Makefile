
# Need to disable flags to use headers of toolchain.
NOSTDINC_FLAGS :=

obj-y += libcomsvc_cfs.a

libcomsvc_cfs-objs := \
	../../src/AmbaCFS.o

libcomsvc_cfs-objs += \
	../../src/AmbaCFS_Search.o	\
	../../src/AmbaCFS_Cache.o	\
	../../src/AmbaCFS_Sch.o	\
	../../src/AmbaCFS_ActiveDta.o	\
	../../src/AmbaCFS_Util.o


# Dependency of AmbaVer.o
$(obj)/AmbaVer.o: $(call amba_ver_dep,AmbaVer.o,$(libcomsvc_cfs-objs))

CFLAGS_AmbaVer.o = -DAMBA_BUILD_DATE=$(AMBA_BUILD_DATE)		\
		   -DAMBA_BUILD_DATE_NUM=$(AMBA_BUILD_DATE_NUM)	\
		   -DAMBA_BUILD_SEC_NUM=$(AMBA_BUILD_SEC_NUM)	\
		   -DAMBA_BUILD_MACHINE=$(AMBA_BUILD_MACHINE)	\
		   -DAMBA_CI_COUNT=$(AMBA_CI_COUNT)		\
		   -DAMBA_CI_ID=$(AMBA_CI_ID)

ccflags-y += -Ivendors/ambarella/inc	\
			-Ivendors/ambarella/inc/io	\
			-Isvc/comsvc/print \
			-Isvc/comsvc/misc \
			-I$(srctree)/$(src)/../../inc	\
			-I$(srctree)/$(src)/../../src

ccflags-y += -Ibsp/$(AMBA_CHIP_ID)/$(AMBA_BSP_NAME)

install-files := libcomsvc_cfs.a


