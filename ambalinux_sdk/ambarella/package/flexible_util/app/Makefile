# SPDX-License-Identifier: GPL-2.0

PWD	:= $(shell pwd)
MOD_PATH = $(PWD)/../mod

CC	?= gcc
CFLAGS	?= -g -ggdb
CFLAGS	+= -I$(MOD_PATH)

OBJS	:= amba_debug.o
APP	:= amba_debug_app

PHONY += all
all: $(OBJS)
	$(Q)$(CC) -o $(APP) $(OBJS) -L$(PWD)

%.o: %.c
	$(Q)$(CC) $(CFLAGS) -c $< -o $@


PHONY += clean
clean:
	$(Q)rm -f $(OBJS) $(APP) *.a *.so.* *.so *.out *.o

.PHONY: $(PHONY)

