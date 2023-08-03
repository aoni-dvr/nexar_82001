
KBUILD_CFLAGS += -I$(srctree)/$(src)/inc
ifneq ($(CONFIG_CC_GCOV),)
KBUILD_CFLAGS += -fprofile-arcs -ftest-coverage
endif

obj-y += crt/
obj-y += libmain.a
# Link script
obj-y += amba.lds

libmain-objs := main.o
ifdef CONFIG_CC_CXX_SUPPORT
libmain-objs += cxx/AmbaCRT0.o \
		cxx/AmbaCtorDtor.o \
		cxx/cpp_case.o
endif

ccflags-y += -I$(srctree)/vendors/ambarella/inc

install-files := libmain.a

