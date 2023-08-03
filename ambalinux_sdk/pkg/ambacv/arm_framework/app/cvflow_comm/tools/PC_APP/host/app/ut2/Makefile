SRCDIR = $(abspath ../../..)
export SRCDIR

ifndef BUILDDIR
BUILDDIR = $(SRCDIR)/build
endif
export BUILDDIR

APP     = $(notdir $(abspath .))
OBJDIR  = $(BUILDDIR)/output/$(APP)

CFLAGS += -Wall -O3 -g -fPIC -I$(SRCDIR)/common/inc
LFLAGS += -pthread

# setup link env for hostapi shared lib
HOSTAPI_LIB_PATH  = $(SRCDIR)/build
HOSTAPI_INC_PATH  = $(SRCDIR)/host/inc
CFLAGS += -I$(HOSTAPI_INC_PATH)
LFLAGS += -L$(HOSTAPI_LIB_PATH) -Wl,-rpath,$(HOSTAPI_LIB_PATH) -lhostapi

SRCS = $(wildcard *.c)
OBJS = $(addprefix $(OBJDIR)/,$(SRCS:.c=.o))

all: $(APP)

$(APP): $(OBJS)
	@echo "        Building:     $@"
	@mkdir -p $(BUILDDIR)
	@$(CC) -o $(BUILDDIR)/$(APP) $(OBJS) $(LFLAGS)
# CV related file
	@cp -a ./data_pad32.bin        $(BUILDDIR)/data_pad32.bin
	@cp -a ./golden_mbox_loc.out $(BUILDDIR)/golden_mbox_loc.out
	@cp -a ./golden_mbox_conf_flatten.out $(BUILDDIR)/golden_mbox_conf_flatten.out
	@cp -a ./settings.txt $(BUILDDIR)/settings.txt

-include $(OBJS:.o=.d)

$(OBJDIR)/%.o: %.c
	@echo "        Compiling:    $@"
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<
	@echo -n $(dir $@) > $(OBJDIR)/$*.d
	@$(CC) -MM $(CFLAGS) $< >> $(OBJDIR)/$*.d
