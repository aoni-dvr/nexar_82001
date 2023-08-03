FORCE:

# Make does not offer a recursive wildcard function, so here's one:
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

################################################################################
#   include user-specifed cvtask list, or default cvtask list otherwise
################################################################################
ifdef CVTASK_LIST_INC
include $(CVTASK_LIST_INC)
else
include cvtask_list.inc
endif

################################################################################
#   create two mutually exclusive lists, if they are not specified by user:
#       CVTASK_WITH_SRC: cvtasks that are build from source
#       CVTASK_FROM_REL: cvtasks that are provided from release library
################################################################################
ifndef CVTASK_WITH_SRC
ifndef CVTASK_FROM_LIB
CVTASK_TMP_AA   := $(sort $(dir $(call rwildcard,$(FMWDIR)/cvtask/,*)))
CVTASK_ACCESSES := $(patsubst $(FMWDIR)/cvtask/%/,%, $(CVTASK_TMP_AA))
CVTASK_WITH_SRC := $(filter     $(CVTASK_ACCESSES), $(CVTASK_MODULES))
CVTASK_FROM_LIB := $(filter-out $(CVTASK_ACCESSES), $(CVTASK_MODULES))
endif
endif


################################################################################
#   define absolute-path list for cvtable binaries:
#       SCVTASK_TBAR: for cvtasks built from source
#       LCVTASK_TBAR: for cvtasks from prebuild release
################################################################################
SCVTASK_TBAR :=
LCVTASK_TBAR = $(foreach mod,$(CVTASK_FROM_LIB), \
	     $(call rwildcard,$(PREBUILDDIR)/cvtask/$(mod)/,*.tbar))

define ADD_TBAR_TARGET
$(1)_make = $$(call rwildcard,$(FMWDIR)/cvtask/$(1)/,makefile)
$(1)_Make = $$(call rwildcard,$(FMWDIR)/cvtask/$(1)/,Makefile)
$(1)_TASK = $$(patsubst $(FMWDIR)/cvtask/%/makefile,%, $$($(1)_make)) \
	    $$(patsubst $(FMWDIR)/cvtask/%/Makefile,%, $$($(1)_Make))
$(1)_TBAR = $$(foreach t,$$($(1)_TASK), \
	$$(if $$(wildcard $(FMWDIR)/cvtask/$$t/$$(notdir $$t).mnft), \
		$(BUILDDIR)/bin/lib/cvtask/$$t/$$(notdir $$t).tbar,))

SCVTASK_TBAR += $$($(1)_TBAR)

$$($(1)_TBAR): FORCE
	@$(MAKE) -C $(FMWDIR)/cvtask \
		$$(dir $$(patsubst $(BUILDDIR)/bin/lib/cvtask/%,%,$$@)).tbar
endef
$(foreach m,$(CVTASK_WITH_SRC),$(eval $(call ADD_TBAR_TARGET,$m)))

CVTASK_TBAR = $(SCVTASK_TBAR) $(LCVTASK_TBAR)

CVTABLE = $(notdir $(basename $(wildcard *.mnft))) \
	  $(notdir $(basename $(wildcard $(BUILDDIR)/../arm/*.mnft)))

ifneq ("$(strip $(CVTABLE))", "")
APP_TBAR := $(foreach t,$(CVTABLE), $(BINDIR)/$t.tbar)
else
APP_TBAR := $(BINDIR)/cvtasks.tbar
endif

cvtable: $(APP_TBAR)

$(BINDIR)/cvtasks.tbar: $(CVTASK_TBAR)
	@echo "        Creating:     $@"
	@mkdir -p $(dir $@)
	test -z $(strip $(CVTASK_TBAR)) || table_ar $@ -cat $(CVTASK_TBAR)

$(BINDIR)/%.tbar: %.mnft $(CVTASK_TBAR)
	@echo "        Creating:     $@"
	@mkdir -p $(dir $@)
	@table_ar -c $@ -a $<
	test -z $(strip $(CVTASK_TBAR)) || table_ar $@ -cat $(CVTASK_TBAR)

$(BINDIR)/%.tbar: $(BUILDDIR)/../arm/%.mnft $(CVTASK_TBAR)
	@echo "        Creating:     $@"
	@mkdir -p $(dir $@)
	@table_ar -c $@ -a $<
	test -z $(strip $(CVTASK_TBAR)) || table_ar $@ -cat $(CVTASK_TBAR)
