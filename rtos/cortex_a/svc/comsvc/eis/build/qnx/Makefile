EXTRA_SRCVPATH := \
		  $(CURDIR)/../../lib/src \
          
          
#EXTRA_SRCVPATH := \
#		  $(CURDIR)/../../../lib/src
SRCS := 
SRCS +=  AmbaEIS_Basic.o
SRCS +=  AmbaEIS_Control.o
SRCS +=  AmbaEIS_SystemApi.o


EXTRA_INCVPATH := \
            $(CURDIR)/../../inc \
			$(CURDIR)/../../lib/inc \
			$(CURDIR)/../../../../../vendors/ambarella/inc \
			$(CURDIR)/../../../../../vendors/ambarella/inc/dsp \
			$(CURDIR)/../../../../../vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID) \
			$(CURDIR)/../../../../../vendors/ambarella/inc/io \
			$(CURDIR)/../../../../../vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
			$(CURDIR)/../../../../../vendors/ambarella/inc/perif \
			$(CURDIR)/../../../../../svc/comsvc/misc \
			$(CURDIR)/../../../../../svc/comsvc/print \
			$(CURDIR)/../../../../../svc/comsvc/imumanager/inc \
			$(CURDIR)/../../../../../svc/comsvc/warputility \
			$(CURDIR)/../../../../../bsp/$(AMBA_CHIP_ID)/peripherals/imu/inc

#EXTRA_INCVPATH += $(srctree)/bsp/$(AMBA_CHIP_ID)/peripherals/imu/inc
#EXTRA_INCVPATH += $(srctree)/svc/comsvc/misc
#EXTRA_INCVPATH += $(srctree)/svc/comsvc/eis/inc
#EXTRA_INCVPATH += $(srctree)/svc/comsvc/eis/lib/inc
#EXTRA_INCVPATH += $(srctree)/svc/comsvc/print
#EXTRA_INCVPATH += $(srctree)/svc/comsvc/imumanager/inc
#EXTRA_INCVPATH += $(srctree)/svc/comsvc/warputility
#
#EXTRA_INCVPATH += $(srctree)/vendors/ambarella
#EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc
#EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp
#EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID)
#EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io
#EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
#EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc/perif
                  


LIST=CPU
include recurse.mk
