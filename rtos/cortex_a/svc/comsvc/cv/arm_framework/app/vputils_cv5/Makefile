################################################################################
#                                This is ThreadX build
ifeq ($(CONFIG_BUILD_CV_THREADX),y)
################################################################################
ifeq ($(CONFIG_CV_CONFIG_TX_SDK7),y)
obj-y += libcv_vputils.a

libcv_vputils-objs := vpstatus.o
libcv_vputils-objs += os/os_wrap.o

ccflags-y := -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/dsp
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/comsvc/misc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/platform
ccflags-y += -I$(srctree)/vendors/ambarella/inc/platform/prfile2
ccflags-y += -I$(srctree)/vendors/ambarella/inc/platform/threadx
ccflags-y += -I$(srctree)/platform/inc/driver/rtsl
ccflags-y += -I$(srctree)/platform/inc
ccflags-y += -I$(srctree)/platform/inc/driver
ccflags-y += -I$(srctree)/platform/fs/inc
ccflags-y += -I$(srctree)/comsvc/printf
ccflags-y += -I$(srctree)/comsvc/misc
ccflags-y += -I$(srctree)/comsvc/shell/inc
ccflags-y += -I$(srctree)/cv/cv_common/inc
ccflags-y += -I$(srctree)/cv/arm_framework/app/vputils/os
ccflags-y += -I$(srctree)/cv/arm_framework/app/vputils/include
ccflags-y += -I$(OUTDIR)
ccflags-y += -O3 -g

install-files := libcv_vputils.a
clean-files += libcv_vputils.a $(libcv_vputils-objs)
else
APPPATH := $(abspath $(lastword $(MAKEFILE_LIST)/..))
APP := $(notdir $(abspath $(lastword $(MAKEFILE_LIST)/..)))
obj-y += libcv_$(APP).a

libcv_$(APP)-objs := vpstatus.o
libcv_$(APP)-objs += os/os_wrap.o

ccflags-y := -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io
ccflags-y += -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID)
ccflags-y += -I$(srctree)/vendors/ambarella/inc/vision
ccflags-y += -I$(srctree)/vendors/ambarella/inc/prfile2
ccflags-y += -I$(srctree)/vendors/ambarella/inc/threadx
ccflags-y += -I$(srctree)/soc/io/src/common/inc/arm/
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/reg
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/csl
ccflags-y += -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl
ccflags-y += -I$(srctree)/$(CV_CORE_DIR)/cv_common/inc
ccflags-y += -I$(srctree)/$(CV_CORE_DIR)/cv_common/inc/cvapi
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -I$(srctree)/svc/comsvc/shell/inc
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/$(CV_COMSVC_DIR)/cv_common/inc/cvapi
ccflags-y += -I$(APPPATH)/os
ccflags-y += -I$(APPPATH)/include
ccflags-y += -I$(OUTDIR)
ccflags-y += -O3 -g

install-files := libcv_$(APP).a
clean-files += libcv_$(APP).a $(libcv_$(APP)-objs)
endif

################################################################################
#                                  This is Linux build
else
################################################################################

ifndef APP
APP := $(notdir $(abspath $(lastword $(MAKEFILE_LIST)/..)))
endif

LMKDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
TOPDIR := $(abspath $(LMKDIR)/../../..)
MKNAME := $(notdir $(lastword $(MAKEFILE_LIST)))
SCHEDULERDIR = $(TOPDIR)/arm_framework/scheduler

ifndef CMNDIR
CMNDIR = $(TOPDIR)/cv_common
endif

ifndef BUILDDIR
BUILDDIR = $(TOPDIR)/build
endif

TOP_BINDIR = $(BUILDDIR)/bin

OBJDIR = $(BUILDDIR)/output/app/$(APP)
BINDIR = $(TOP_BINDIR)/app/$(APP)

CFLAGS  += -Wall -O3 -g -fPIC
CFLAGS  += -I./include -I./os
CFLAGS  += -I$(CMNDIR)/inc  -I$(CMNDIR)/inc/cvapi -I$(CMNDIR)/inc/cvsched_common -I$(SCHEDULERDIR)/os/linux/ -I$(SCHEDULERDIR)/inc -I$(SCHEDULERDIR)/inc/linux

LFLAGS   = -L$(TOP_BINDIR)/lib -pthread -lambadag -lm -lrt
ifdef CV_COMMON_LFLAGS
LFLAGS += $(CV_COMMON_LFLAGS)
endif

ifeq ("$(ENABLE_AMBA_MAL)","y")
CFLAGS += -DENABLE_AMBA_MAL
endif

VPSTATUS_SRC   =  vpstatus.c
VPSTATUS_SRC  +=  os/os_wrap.c

VPSTATUS_OBJ   =  $(addprefix $(OBJDIR)/, $(VPSTATUS_SRC:.c=.o))



APP_NAME = vpstatus

all: app $(addprefix $(BINDIR)/, $(APP_NAME))

-include $(patsubst %,$(OBJDIR)/%.d, $(APP_NAME))
include $(CMNDIR)/build/linker_script.inc

app: $(BINDIR)/$(APP_NAME)
$(BINDIR)/$(APP_NAME): $(VPSTATUS_OBJ) $(APP_LSR)
	@echo "  Linking $@"
	@echo "  obj $(VPSTATUS_OBJ)"
	@echo "  BINDIR $(BINDIR)"
	@mkdir -p $(dir $@)
	$(CC) -o $@ $(VPSTATUS_OBJ) -T $(APP_LSR) $(LFLAGS)


clean:
	rm -rf $(OBJDIR) vpstatus

-include $(VPSTATUS_OBJ:.o=.d)

$(OBJDIR)/%.o: %.c
	@echo "        Compiling:    $@"
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<
	@echo -n $(dir $@) > $(OBJDIR)/$*.d
	@$(CC) -MM $(CFLAGS) $< >> $(OBJDIR)/$*.d

################################################################################
#                                end of file
endif
################################################################################
