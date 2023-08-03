PWD	:= $(shell pwd)

BUILD_SHA1 := $(git describe --dirty)
BUILD_USER := $(shell whoami)@$(shell hostname)
BUILD_TIME := $(shell date)

EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/ambamal/inc
EXTRA_CFLAGS            += -I$(AMBARELLA_PKG_DIR)/ambamal/linux

obj-m 	+= ambamal.o

ambamal-y := AmbaMAL_Dev.o     \
			 AmbaMAL_Mmb.o     \
			 AmbaMAL_CMA.o	   \
			 AmbaMAL_CMAOps.o   \
			 AmbaMAL_AMAOps.o   \
			 AmbaMAL_Krn.o
