ifndef BUILDDIR
export BUILDDIR := $(abspath ../build)
endif

CMNDIR  := $(abspath ../cv_common)

ifndef PREBUILDDIR
PREBUILDDIR := $(shell cat $(CMNDIR)/.release_lib_root)/arm/pace
endif

export CMNDIR
export BUILDDIR
export PREBUILDDIR
export APP_TYPE=pace
export CV_OSTYPE=PACE_LINUX
export STATIC_LINK=y

include cvchip.mk

APPS = $(subst app/,,$(wildcard app/*))
APPS_ALL = $(addsuffix .all, $(APPS))

.PHONY: scheduler cvtask cvlib framework app $(APPS) $(APPS_ALL)

all:  app

framework: scheduler

scheduler:
	@if [ -d "./scheduler" ]; then $(MAKE) -C ./scheduler CVCHIP=$(CVCHIP) CVARCH=$(CVARCH); fi

cvtask:
	@$(MAKE) -C ./cvtask all CVCHIP=$(CVCHIP) CVARCH=$(CVARCH)

cvlib:
	@$(MAKE) -C ./cvlib all CVCHIP=$(CVCHIP) CVARCH=$(CVARCH)

app: $(APPS)

$(APPS): framework
	@$(MAKE) -C ./app/$@ app CVCHIP=$(CVCHIP) CVARCH=$(CVARCH)

$(APPS_ALL): framework
	@$(MAKE) -C ./app/$(basename $@) all CVCHIP=$(CVCHIP) CVARCH=$(CVARCH)

################################################################################
#
#                              Installing libraries
#
################################################################################
CVTASKS := $(filter-out makefile, $(subst cvtask/,,$(wildcard cvtask/*)))
INSSRC   = $(BUILDDIR)/bin/lib

install: scheduler cvtask
	@echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	@install -m 0775 -d $(INSDST)
	@install -m 0664 $(INSSRC)/libambadag.a $(INSDST)
	@install -m 0775 -d $(INSDST)/cvtask
	@for t in $(CVTASKS); do \
		install -m 0775 -d $(INSDST)/cvtask/$$t; \
		install -m 0664 $(INSSRC)/cvtask/$$t/* $(INSDST)/cvtask/$$t; \
	done

clean:
	$(RM) -rf $(BUILDDIR)
