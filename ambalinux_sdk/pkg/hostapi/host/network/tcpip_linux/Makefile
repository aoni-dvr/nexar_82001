OBJDIR  = $(BUILDDIR)/output/lib
SRCS    = $(wildcard *.c)
OBJS    = $(addprefix $(OBJDIR)/,$(SRCS:.c=.o))
LIB     = $(BUILDDIR)/libhost_network.a

CFLAGS += -Wall -O3 -g -fPIC -I$(SRCDIR)/common/inc -I$(SRCDIR)/host/inc

all: $(OBJS)

$(LIB): $(OBJS)
	@echo "        Building Lib: $@"
	@mkdir -p $(dir $@)
	@$(AR) rcs $@ $<

-include $(OBJS:.o=.d)

$(OBJDIR)/%.o: %.c
	@echo "        Compiling:    $@"
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<
	@echo -n $(dir $@) > $(OBJDIR)/$*.d
	@$(CC) -MM $(CFLAGS) $< >> $(OBJDIR)/$*.d
