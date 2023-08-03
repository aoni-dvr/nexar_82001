ifeq ($(CONFIG_BUILD_MAL),y)

obj-y := libmal.a

libmal-objs += ../../threadx/AmbaMAL_ThreadX.o

ccflags-y += \
        -I$(srctree)/vendors/ambarella/inc/ \
        -I$(srctree)/vendors/ambarella/inc/io \
        -I$(srctree)/svc/comsvc/print \
        -I$(srctree)/svc/comsvc/misc \

asflags-y += \

install-files := libmal.a

endif
