#
# Copyright (c) 2017-2017 Ambarella, Inc.
#
# This file and its contents ("Software") are protected by intellectual property rights including,
# without limitation, U.S. and/or foreign copyrights.  This Software is also the confidential and
# proprietary information of Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
# distribute, modify, or otherwise prepare derivative works of this Software or any portion thereof
# except pursuant to a signed license agreement or nondisclosure agreement with Ambarella, Inc. or
# its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
# return this Software to Ambarella, Inc.
#
# THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
# TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
# OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
ifndef APP
APP := $(notdir $(abspath $(lastword $(MAKEFILE_LIST)/..)))
endif

.PHONY: $(APP) sysflow cvtable mpxcopy

LMKDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
TOPDIR := $(abspath $(LMKDIR)/../../..)
FMWDIR := $(TOPDIR)/arm_framework
MKNAME := $(notdir $(lastword $(MAKEFILE_LIST)))

ifndef CMNDIR
CMNDIR = $(TOPDIR)/cv_common
endif

ifndef BUILDDIR
BUILDDIR = $(TOPDIR)/build
endif

# pull in HOSTAPI source files
ifndef HOSTAPI
HOSTAPI = $(abspath $(wildcard $(TOPDIR)/../hostapi*))
endif

TOP_BINDIR = $(BUILDDIR)/bin

OBJDIR = $(BUILDDIR)/output/app/$(APP)
BINDIR = $(TOP_BINDIR)/app/$(APP)

APP_INCS := -I$(CMNDIR)/inc -I$(CMNDIR)/inc/cvapi -I$(FMWDIR)/scheduler/inc
APP_INCS += -I$(HOSTAPI)/common/inc -I$(HOSTAPI)/target/inc
APP_INCS += -I$(CMNDIR)/stream

APP_SRCS := $(wildcard $(LMKDIR)/*.c)
APP_SRCS += $(wildcard $(HOSTAPI)/target/network/**/*.c)

all: app sysflow mpxcopy cvtable

include $(CMNDIR)/build/cvtask_lib.inc
include $(CMNDIR)/build/cvtable.inc
include $(CMNDIR)/build/sysflow.inc
include $(CMNDIR)/build/mpxcopy.inc
include $(CMNDIR)/build/linker_script.inc

CFLAGS := $(APP_INCS)

LFLAGS := -L$(TOP_BINDIR)/lib -L$(PREBUILDDIR)
LFLAGS += -pthread -lambadag -lm -lrt
ifdef CV_COMMON_LFLAGS
LFLAGS += $(CV_COMMON_LFLAGS)
endif
LFLAGS += -Xlinker -Map=$(OBJDIR)/$(APP).map
ifeq ($(ENABLE_CVTASK_CAMCTRL_IPC),y)
LFLAGS +=  -lambaipc -laipc_cfg
else ifeq ($(ENABLE_CVTASK_CAMCTRL_IPC_EVA),y)
LFLAGS +=  -leva_module
endif

APP_OBJS   = $(subst $(LMKDIR),$(OBJDIR),$(APP_SRCS:.c=.o))

ifneq ("$(wildcard $(TOP_BINDIR)/lib/libambadag.a)","")
SCHEDULER_LIB = $(TOP_BINDIR)/lib/libambadag.a
endif

app: $(BINDIR)/$(APP)
$(BINDIR)/$(APP): $(APP_OBJS) $(APP_LSR) $(SCVTASK_LIBS) $(SCVLIB_LIBS) $(SCHEDULER_LIB)
	@echo "=======> Building Application #$@#"
	@mkdir -p $(dir $@)
	$(CC) -o $@ -T $(APP_LSR) $(APP_OBJS) $(LFLAGS) $(CVTASK_LIBS_FLAGS)
ifeq ("$(STATIC_LINK)","n")
ifneq ("$(LCVTASK_LIBS) $(LCVLIB_LIBS)"," ")
	install -m 755 -D -t $(BUILDDIR)/bin/lib/prebuild $(LCVTASK_LIBS) $(LCVLIB_LIBS)
endif
endif

-include $(APP_OBJS:.o=.d)

$(OBJDIR)/%.o: $(LMKDIR)/%.c
	@echo "=======> Compiling:  $<"
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<
	@echo -n $(dir $@) > $(OBJDIR)/$*.d
	@$(CC) -MM $(CFLAGS) $< >> $(OBJDIR)/$*.d

