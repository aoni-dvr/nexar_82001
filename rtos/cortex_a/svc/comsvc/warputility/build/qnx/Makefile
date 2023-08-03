EXTRA_SRCVPATH := \
		  $(CURDIR)/../../src

SRCS := \
	AmbaWU_WarpUtility.o \
	AmbaWU_SystemApi.o \
	AmbaWU_WarpExecuteUtility.o \
	
EXTRA_INCVPATH := \
$(CURDIR)/../.. \
$(CURDIR)/../../inc \
$(CURDIR)/../../../../../vendors/ambarella/inc \
$(CURDIR)/../../../../../vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID) \
$(CURDIR)/../../../../../svc/comsvc/warputility \
$(CURDIR)/../../../../../svc/comsvc/warputility/inc \
$(CURDIR)/../../../../../svc/comsvc/print \
$(CURDIR)/../../../../../vendors/ambarella/wrapper/kal/qnx/public

LIST=CPU
include recurse.mk
