################################################################
#                             Linux                            #
################################################################
BINDIR   := $(BUILDDIR)/bin
OBJDIR   := $(BUILDDIR)/output/scheduler

CFLAGS  += -Wall -O3 -g -fPIC -I$(OBJDIR)
CFLAGS  += -I$(CMNDIR)/inc -I$(CMNDIR)/inc/cvapi -I$(CMNDIR)/inc/cavalry -I$(CMNDIR)/inc/cvsched_common -I./inc -I./core/linux -D$(CVCHIP) -D$(CVARCH)
LFLAGS   = -L$(BINDIR) -pthread

DAGSRC   =  $(wildcard core/*.c)
DAGSRC  += core/linux/os_api.c    \
	    core/linux/schdr_resource.c

ifeq ("$(CVCHIP)","CHIP_CV2A")
CFLAGS  += -DASIL_COMPLIANCE
else ifeq ("$(CVCHIP)","CHIP_CV2FS")
CFLAGS  += -DASIL_COMPLIANCE
else ifeq ("$(CVCHIP)","CHIP_CV22A")
CFLAGS  += -DASIL_COMPLIANCE
else ifeq ("$(CVCHIP)","CHIP_CV22FS")
CFLAGS  += -DASIL_COMPLIANCE
endif

ifeq ("$(AMBACV_KERNEL_SUPPORT)","y")
CFLAGS  += -DAMBACV_KERNEL_SUPPORT
DAGSRC  +=  core/linux/debug_core.c core/linux/set_roi.c
ifeq ("$(ENABLE_AMBA_MAL)","y")
CFLAGS += -DENABLE_AMBA_MAL
endif
else
DAGSRC  += $(wildcard core/linux/amalgam/*.c)
CFLAGS  += -DPACE_DIAG
endif

DAGOBJ   =  $(addprefix $(OBJDIR)/, $(DAGSRC:.c=.o))
ifeq ("$(STATIC_LINK)","y")
DAGLIB   = $(BINDIR)/lib/libambadag.a
else
DAGLIB   = $(BINDIR)/lib/libambadag.so
endif

all:
	@mkdir -p $(OBJDIR)
	@$(CMNDIR)/build/create_build_version.sh $(CURDIR) $(OBJDIR) $(CMNDIR)
	@$(MAKE) $(DAGLIB)

$(DAGLIB): $(DAGOBJ)
	@echo "        Building Lib: $@"
	@mkdir -p $(dir $@)
ifeq ("$(STATIC_LINK)","y")
	@$(AR) rcs $@ $(DAGOBJ)
else
	@$(CC) -shared -Wl,--as-needed -Wl,--warn-common -o $@ $(DAGOBJ)
endif

clean:
	rm -rf $(OBJDIR) $(DAGLIB)

-include $(DAGOBJ:.o=.d)

$(OBJDIR)/%.o: %.c
	@echo "        Compiling:    $@"
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<
	@echo -n $(dir $@) > $(OBJDIR)/$*.d
	@$(CC) -MM $(CFLAGS) $< >> $(OBJDIR)/$*.d

