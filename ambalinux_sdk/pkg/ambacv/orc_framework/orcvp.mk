DIAG_DIR := $(AMBARELLA_PKG_DIR)/ambacv/cv_common/libs/orc

ifeq ($(PROJECT), cv6)
ORC_NAME = visorc
else
ORC_NAME = orcvp
endif

ifneq ("$(wildcard $(DIAG_DIR))","")
BUILDDIR := $(abspath ../../../diags)
BUILDDIR_ORCVP := $(abspath ../../../diags/orc_scheduler/bin/visorc)
BUILDDIR_CVTABLE := $(abspath ../../../diags/orc_scheduler/orc/build)
INSDIR  = $(DESTDIR)/lib/firmware

all: orcvp

orcvp:
	@

install:
	@echo "Copying flexidag common orcvp.bin to target rootfs"
	install -D -m 644 $(DIAG_DIR)/scheduler/$(PROJECT)_$(ORC_NAME)/orcvp.bin $(INSDIR)/orcvp.bin
	install -D -m 644 $(DIAG_DIR)/scheduler/$(PROJECT)_$(ORC_NAME)/visorc_cvtable.tbar $(INSDIR)/visorc_cvtable.tbar

else  #if $(DIAG_DIR)

all:
install:

endif #if $(DIAG_DIR)
