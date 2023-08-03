obj-y           := libfs_$(AMBA_CHIP_ID).a
install-files   := libfs_$(AMBA_CHIP_ID).a

libfs_$(AMBA_CHIP_ID)-objs += \
        AmbaFS_linux.o

ccflags-y += \
        -I$(srctree)/vendors/ambarella/inc \
        -I$(srctree)/vendors/ambarella/inc/io \
        -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
        -I$(srctree)/soc/io/src/common/inc \
        -I$(srctree)/soc/io/src/common/inc/arm \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc\
		-I$(srctree)/vendors/ambarella/inc/linux\
		-I$(srctree)/svc/comsvc/misc
