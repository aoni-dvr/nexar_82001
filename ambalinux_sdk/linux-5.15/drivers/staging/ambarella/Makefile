# drivers/staging/ambarella/Makefile
#
# Copyright (C) 2021, Ambarella Inc.

EXTRA_CFLAGS +=	-DUSE_API_STRUCT_API_NAMES

obj-$(CONFIG_ARMHSM) += ambahsm.o
ambahsm-y := armhsm.o comm.o msgq.o hsmapi.o hsmnvm.o callreply.o
