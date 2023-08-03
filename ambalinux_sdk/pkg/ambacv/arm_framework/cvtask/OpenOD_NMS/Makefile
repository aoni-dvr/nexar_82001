#
# Copyright (c) 2021-2022 Ambarella International LP
#
# This file and its contents ("Software") are protected by intellectual property
# rights including, without limitation, U.S. and/or foreign copyrights. This
# Software is also the confidential and proprietary information of Ambarella
# International LP and its licensors. You may not use, reproduce, disclose,
# distribute, modify, or otherwise prepare derivative works of this Software or
# any portion thereof except pursuant to a signed license agreement or
# nondisclosure agreement with Ambarella International LP or its authorized
# affiliates. In the absence of such an agreement, you agree to promptly notify
# and return this Software to Ambarella International LP.
#
# THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
# MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
# EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
################################################################################
#                                This is ThreadX build
ifeq ($(CONFIG_BUILD_CV),y)
################################################################################
CURDIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
OUTDIR := $(objtree)/$(obj)
CVTASK := $(notdir $(CURDIR))
.NOTPARALLEL:
.PHONY: force_build_info

-include $(CURDIR)/private.mk
$(obj)/build_version.h: force_build_info
	@mkdir -p $(OUTDIR)
	@if [ -f $(BUILDINFO_CMD) ] ; then                                      \
	  $(BUILDINFO_CMD) $(CURDIR) $(OUTDIR) $(CMNDIR);                       \
	fi

ccflags-y += -I$(srctree)/vendors/ambarella/inc                                 \
	     -I$(srctree)/vendors/ambarella/inc/ssp                             \
	     -I$(srctree)/vendors/ambarella/inc/ssp/fs                          \
	     -I$(srctree)/vendors/ambarella/inc/ssp/prfile2                     \
	     -I$(srctree)/vendors/ambarella/inc/ssp/soc                         \
	     -I$(srctree)/vendors/ambarella/inc/comsvc/AmbaSys                  \
	     -I$(srctree)/cv/cv_common/inc                                      \
	     -I$(srctree)/cv/cv_common/inc/cvapi                                \
	     -I$(OUTDIR)							\
	     -DTHREADX_BUILD -O3 -g

obj-y += build_version.h libcv_$(CVTASK).a

ifndef C_SRCS
C_SRCS := $(patsubst $(CURDIR)/%,%,$(wildcard $(CURDIR)/*.c))
endif

libcv_$(CVTASK)-objs = $(C_SRCS:%.c=%.o)
install-files += libcv_$(CVTASK).a

################################################################################
#                                  This is Linux build
else
################################################################################
CVTDIR := $(patsubst $(CVROOT)/%,%,$(CURDIR))
CVTASK := $(notdir $(CVTDIR))
.PHONY: $(CVTASK)

-include private.mk

all: lib tbar

ifneq ("$(wildcard $(CVTASK).mnft)","")
TBAR    = $(BINDIR)/cvtask/$(CVTDIR)/$(CVTASK).tbar
$(TBAR):  $(CVTASK).mnft
	@echo "        Building:     $@"
	@mkdir -p $(dir $@)
	@table_ar -c $@ -a $<
endif
tbar: $(TBAR)

ifndef SCHDAPI
SCHDAPI = $(abspath $(CVROOT)/../scheduler)
endif

ifndef HOSTAPI
HOSTAPI = $(abspath $(wildcard $(CVROOT)/../../../hostapi*))
endif

ifndef CMNDIR
CMNDIR = $(abspath $(CVROOT)/../../cv_common)
endif

ifndef C_SRCS
C_SRCS = $(wildcard *.c)
endif

ifndef CPP_SRCS
CPP_SRCS = $(wildcard *.cpp)
endif

OUTDIR  = $(OBJDIR)/$(CVTDIR)
OBJS    = $(addprefix $(OUTDIR)/, $(C_SRCS:.c=.o))
OBJS   += $(addprefix $(OUTDIR)/, $(CPP_SRCS:.cpp=.o))
LIB     = $(BINDIR)/cvtask/$(CVTDIR)/lib$(CVTASK).a
ifneq ("$(MM_AR)", "")
MM_OBJS += $(shell $(AR) t $(MM_AR))
endif

$(LIB): $(OBJS)
	@echo "        Creating:     $@"
	@mkdir -p $(dir $@)
ifneq ("$(MM_AR)", "")
	@$(AR) x $(MM_AR)
endif
	@$(AR) rcs $@ $(OBJS) $(MM_OBJS) $(wildcard *.o)


lib:
	@mkdir -p $(OUTDIR)
	@$(CMNDIR)/build/create_build_version.sh $(CURDIR) $(OUTDIR) $(CMNDIR)
	$(MAKE) $(LIB)

ifneq ("$(MAKECMDGOALS)","tbar")
-include $(OBJS:.o=.d)
endif
MM_TV_INCS := $(shell tv2 -incpath AmbaCnn)

CFLAGS += -I$(OUTDIR) -D$(CV_OSTYPE) -I$(SCHDAPI)/inc -I$(HOSTAPI)/target/inc -I$(HOSTAPI)/common/inc -I$(CMNDIR)/stream -I$(MM_TV_INCS)

ifeq ($(CVTASK_RELEASE_BUILD),1)
CFLAGS += -DCVTASK_RELEASE_BUILD
endif

$(OUTDIR)/%.o: %.c
	@echo "        Compiling:    $(CURDIR)/$<"
	@mkdir -p $(dir $@)
	@$(CC) -c $(CFLAGS) -o $@ $<
	@echo -n $(dir $@) > $(OUTDIR)/$*.d
	@echo "Using in extern makefile : $(CC)"
	@$(CC) -MM $(CFLAGS) $< >> $(OUTDIR)/$*.d

$(OUTDIR)/%.o: %.cpp
	@echo "        Compiling:    $(CURDIR)/$<"
	@mkdir -p $(dir $@)
	@$(CXX) -c $(CXXFLAGS) -o $@ $<
	@echo -n $(dir $@) >> $(OUTDIR)/$*.d
	@$(CXX) -MM $(CXXFLAGS) $< >> $(OUTDIR)/$*.d


################################################################################
#                                end of file
endif
################################################################################
