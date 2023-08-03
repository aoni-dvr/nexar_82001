export BINDIR := $(BUILDDIR)/bin/lib
export OBJDIR := $(BUILDDIR)/output/cvtask
export CFLAGS := -Wall -O3 -g -fPIC -I$(CMNDIR)/inc -I$(CMNDIR)/inc/cvapi -I$(CMNDIR)/inc/cavalry -D$(CVCHIP) -D$(CVARCH)
export CVROOT := $(realpath $(CURDIR))

ifeq ("$(PROJECT)","cv1")
CFLAGS  += -DCHIP_CV1
endif

ifeq ("$(PROJECT)","cv2")
CFLAGS  += -DCHIP_CV2
endif

ifeq ("$(PROJECT)","cv22")
CFLAGS  += -DCHIP_CV22
endif

ifeq ("$(PROJECT)","cv2s")
CFLAGS  += -DCHIP_CV22
endif

ifeq ("$(PROJECT)","cv2a")
CFLAGS  += -DCHIP_CV2A
endif

ifeq ("$(PROJECT)","cv2ai")
CFLAGS  += -DCHIP_CV2A
endif

ifeq ("$(PROJECT)","cv22a")
CFLAGS  += -DCHIP_CV22A
endif

ifeq ("$(PROJECT)","cv22ai")
CFLAGS  += -DCHIP_CV22A
endif

ifeq ("$(PROJECT)","cv2fs")
CFLAGS  += -DCHIP_CV2FS
endif

ifeq ("$(PROJECT)","cv22fs")
CFLAGS  += -DCHIP_CV22FS
endif

ifeq ("$(PROJECT)","cv5")
CFLAGS  += -DCHIP_CV5
endif

ifeq ("$(PROJECT)","cv5i")
CFLAGS  += -DCHIP_CV5
endif

ifeq ("$(PROJECT)","cv5s")
CFLAGS  += -DCHIP_CV5
endif

ifeq ("$(PROJECT)","cv52")
CFLAGS  += -DCHIP_CV52
endif

ifeq ("$(PROJECT)","cv52s")
CFLAGS  += -DCHIP_CV52
endif

ifeq ("$(PROJECT)","cv6")
CFLAGS  += -DCHIP_CV6
endif

# Make does not offer a recursive wildcard function, so here's one:
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

MKFILES = $(call rwildcard,$(CVROOT)/,Makefile)
CVTASKS = $(patsubst $(CVROOT)/%/Makefile,%,$(MKFILES))

all: $(addsuffix .all, $(CVTASKS))

%.lib:
	@$(MAKE) -C $(basename $*) lib

%.tbar:
	@$(MAKE) -C $(basename $*) tbar

%.all:
	@$(MAKE) -C $(basename $*) all
