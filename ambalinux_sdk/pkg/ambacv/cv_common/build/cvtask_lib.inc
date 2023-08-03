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
#   define absolute-path list for cvtask library:
#       SCVTASK_LIBS: for cvtasks built from source
#       LCVTASK_LIBS: for cvtasks from prebuild release
################################################################################
SCVTASK_LIBS :=
ifeq ("$(STATIC_LINK)","y")
LCVTASK_LIBS = $(foreach mod,$(CVTASK_FROM_LIB),$(call rwildcard,$(PREBUILDDIR)/cvtask/$(mod)/,lib*.a))
else
LCVTASK_LIBS = $(foreach mod,$(CVTASK_FROM_LIB),$(call rwildcard,$(PREBUILDDIR)/cvtask/$(mod)/,lib*.so))
endif

define ADD_LIB_TARGET
$(1)_make = $$(call rwildcard,$(FMWDIR)/cvtask/$(1)/,makefile)
$(1)_Make = $$(call rwildcard,$(FMWDIR)/cvtask/$(1)/,Makefile)
$(1)_TASK = $$(patsubst $(FMWDIR)/cvtask/%/makefile,%, $$($(1)_make)) $$(patsubst $(FMWDIR)/cvtask/%/Makefile,%, $$($(1)_Make))
ifeq ("$(STATIC_LINK)","y")
$(1)_LIBS = $$(foreach t,$$($(1)_TASK), $(BUILDDIR)/bin/lib/cvtask/$$t/lib$$(notdir $$t).a)
else
$(1)_LIBS = $$(foreach t,$$($(1)_TASK), $(BUILDDIR)/bin/lib/cvtask/$$t/lib$$(notdir $$t).so)
endif

SCVTASK_LIBS += $$($(1)_LIBS)

$$($(1)_LIBS): FORCE
	@$(MAKE) -C $(FMWDIR)/cvtask \
		$$(dir $$(patsubst $(BUILDDIR)/bin/lib/cvtask/%,%,$$@)).lib
endef
$(foreach m,$(CVTASK_WITH_SRC),$(eval $(call ADD_LIB_TARGET,$m)))

cvtask: $(SCVTASK_LIBS)

################################################################################
#   include user-specifed cvlib list, or none otherwise
################################################################################
ifdef CVLIB_LIST_INC
include $(CVLIB_LIST_INC)
else
CVLIB_LIST_INC  :=
endif

################################################################################
#   create two mutually exclusive lists, if they are not specified by user:
#       CVLIB_WITH_SRC: cvlibs that are build from source
#       CVLIB_FROM_REL: cvlibs that are provided from release library
################################################################################
ifndef CVLIB_WITH_SRC
ifndef CVLIB_FROM_LIB
CVLIB_TMP_AA    := $(sort $(dir $(call rwildcard,$(FMWDIR)/cvlib/,*)))
CVLIB_ACCESSES  := $(patsubst $(FMWDIR)/cvlib/%/,%, $(CVLIB_TMP_AA))
CVLIB_WITH_SRC  := $(filter     $(CVLIB_ACCESSES), $(CVLIB_MODULES))
CVLIB_FROM_LIB  := $(filter-out $(CVLIB_ACCESSES), $(CVLIB_MODULES))
endif
endif

################################################################################
#   define absolute-path list for cvlib library:
#       SCVLIB_LIBS: for cvlibs built from source
#       LCVLIB_LIBS: for cvlibs from prebuild release
################################################################################
SCVLIB_LIBS     :=
ifeq ("$(STATIC_LINK)","y")
LCVLIB_LIBS     = $(foreach mod,$(CVLIB_FROM_LIB),$(call rwildcard,$(PREBUILDDIR)/cvlib/$(mod)/,lib*.a))
else
LCVLIB_LIBS     = $(foreach mod,$(CVLIB_FROM_LIB),$(call rwildcard,$(PREBUILDDIR)/cvlib/$(mod)/,lib*.so))
endif

define ADD_CVLIB_TARGET
$(1)_make   = $$(call rwildcard,$(FMWDIR)/cvlib/$(1)/,makefile)
$(1)_Make   = $$(call rwildcard,$(FMWDIR)/cvlib/$(1)/,Makefile)
$(1)_CVLIB  = $$(patsubst $(FMWDIR)/cvlib/%/makefile,%, $$($(1)_make)) $$(patsubst $(FMWDIR)/cvlib/%/Makefile,%, $$($(1)_Make))
ifeq ("$(STATIC_LINK)","y")
$(1)_LIBS   = $$(foreach t,$$($(1)_CVLIB), $(BUILDDIR)/bin/lib/cvlib/$$t/lib$$(notdir $$t).a)
else
$(1)_LIBS   = $$(foreach t,$$($(1)_CVLIB), $(BUILDDIR)/bin/lib/cvlib/$$t/lib$$(notdir $$t).so)
endif

SCVLIB_LIBS += $$($(1)_LIBS)


$$($(1)_LIBS): FORCE
	@$(MAKE) -C $(FMWDIR)/cvlib $$(dir $$(patsubst $(BUILDDIR)/bin/lib/cvlib/%,%,$$@)).lib
endef

$(foreach m,$(CVLIB_WITH_SRC),$(eval $(call ADD_CVLIB_TARGET,$m)))

cvlib: $(SCVLIB_LIBS)

ifeq ("$(STATIC_LINK)","y")
CVTASK_LIBS_FLAGS = -Wl,--whole-archive              \
		    $(SCVTASK_LIBS) $(LCVTASK_LIBS)  \
		    $(SCVLIB_LIBS) $(LCVLIB_LIBS)  \
		    -Wl,--no-whole-archive
else
CVTASK_LIBS_FLAGS = $(SCVLIB_LIBS) $(SCVTASK_LIBS) $(LCVTASK_LIBS) $(LCVLIB_LIBS)
endif
