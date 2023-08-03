ifeq ($(CONFIG_BUILD_MAL),y)

obj-y := libmal.a

libmal-objs += ../../linux/AmbaMAL_Linux.o

ccflags-y += \
        -I$(srctree)/vendors/ambarella/wrapper/mal/linux/ \
        -I$(srctree)/vendors/ambarella/inc/ \

asflags-y += \

install-files := libmal.a

endif