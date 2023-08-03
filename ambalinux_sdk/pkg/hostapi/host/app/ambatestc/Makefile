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

SRCS = $(wildcard *.c)
OBJS = $(addprefix $(OBJDIR)/,$(SRCS:.c=.o))

all: $(APP)

$(APP): $(OBJS)
	@echo "        Building:     $@"
	@mkdir -p $(BUILDDIR)
	@$(CC) -o $(BUILDDIR)/$(APP) $(OBJS) $(LFLAGS)

-include $(OBJS:.o=.d)

$(OBJDIR)/%.o: %.c
	@echo "        Compiling:    $@"
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<
	@echo -n $(dir $@) > $(OBJDIR)/$*.d
	@$(CC) -MM $(CFLAGS) $< >> $(OBJDIR)/$*.d
