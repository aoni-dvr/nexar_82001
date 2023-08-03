obj-y += libpba.a

# Link script
obj-y += amba_pba.lds

ccflags-y := -I$(srctree)/vendors/ambarella/inc/
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/${AMBA_CHIP_ID}
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp/${AMBA_CHIP_ID}
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/apps/icam/cardv
ccflags-y += -I$(srctree)/svc/apps/icam/main/src/config/inc
ccflags-y += -I$(srctree)/svc/apps/icam/main/src/inc
ccflags-y += -I$(srctree)/svc/apps/icam/core/src/sys/inc
ccflags-y += -I$(srctree)/svc/apps/icam/main/src/entry/threadx
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/svc/comsvc/stereoutility
ccflags-y += -I$(srctree)/svc/comsvc/imgcal/arch
ccflags-y += -I$(srctree)/soc/io/src/${AMBA_CHIP_ID}/inc
ccflags-y += -I$(srctree)/soc/io/src/${AMBA_CHIP_ID}/inc/rtsl
ccflags-y += -I$(srctree)/soc/io/src/common/inc
ccflags-y += -I$(srctree)/soc/io/src/common/inc/rtsl
ccflags-y += -I$(srctree)/bsp/${AMBA_CHIP_ID}/${AMBA_BSP_NAME}
ccflags-y += -I$(srctree)/bsp/${AMBA_CHIP_ID}/peripherals/pmic/inc

libpba-objs += ../svc/apps/icam/main/src/entry/threadx/iCamSD.o
libpba-objs += AmbaUserSysCtrl.o
libpba-objs += AmbaFirmwareUpdater.o
install-files := libpba.a
