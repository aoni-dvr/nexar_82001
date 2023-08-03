
# We link the stack with -E so a lot of the undefined
# references get resolved from the stack itself.  If
# you want them listed at link time, turn off
# --allow-shlib-undefined and replace with --warn-once
# if desired.

#LDFLAGS+=-Wl,--warn-once
LDFLAGS+=-Wl,--allow-shlib-undefined
CCFLAGS += -D_KERNEL -DALTQ -include $(PROJECT_ROOT)/../../../../../output/include/generated/autoconf.h

# gcc sometime after 2.95.3 added a builtin log()
CCFLAGS_gcc += -fno-builtin-log -Wall -Wextra -Werror
# currently doesnt handle individual builtins
CCFLAGS_clang += -fno-builtin
CCFLAGS += $(CCFLAGS_$(COMPILER_TYPE))
