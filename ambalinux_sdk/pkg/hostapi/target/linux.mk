SRCDIR = $(abspath ../)

ifndef BUILDDIR
export BUILDDIR := $(SRCDIR)/build
endif

export SRCDIR
export BUILDDIR

APP     = ambahostd
OBJDIR  = $(BUILDDIR)/output/$(APP)

CFLAGS += -Wall -O3 -g -fPIC -I$(SRCDIR)/common/inc
LFLAGS += -L$(BUILDDIR) -pthread -lhost_core -lhost_network

SRCS = $(wildcard ambahostd/*.c)
OBJS = $(addprefix $(OBJDIR)/,$(SRCS:.c=.o))

all: $(APP)

hostlibs:
	@$(MAKE) -C $(SRCDIR)/target/core
	@$(MAKE) -C $(SRCDIR)/target/network/tcpip_linux

$(APP): $(OBJS) hostlibs
	@echo "        Building:     $@"
	@mkdir -p $(BUILDDIR)
	@$(CC) -o $(BUILDDIR)/$(APP) $(OBJS) $(LFLAGS)

install:
	@echo "Copy ambahostd to target rootfs"
	install -D -m 755 $(BUILDDIR)/$(APP) $(DESTDIR)/usr/bin

-include $(OBJS:.o=.d)

$(OBJDIR)/%.o: %.c
	@echo "        Compiling:    $@"
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<
	@echo -n $(dir $@) > $(OBJDIR)/$*.d
	@$(CC) -MM $(CFLAGS) $< >> $(OBJDIR)/$*.d
