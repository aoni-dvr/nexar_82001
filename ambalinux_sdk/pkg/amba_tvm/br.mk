
LOCAL_PATH = .

CFLAGS	+= -Wall -O2 -g -fPIC \
		   -I$(LOCAL_PATH)/inc \
		   -I$(LOCAL_PATH)/src \
		   -I$(AMBARELLA_PKG_DIR)/ambacv/cv_common/inc/ \
		   -I$(AMBARELLA_PKG_DIR)/ambacv/cv_common/inc/cvapi/ \
		   -I$(AMBARELLA_PKG_DIR)/ambacv/cv_common/inc/rtos/ \
   		   -I${AMBA_DLR_ROOT_DIR}/prebuild/amba/include \
   		   -I${AMBA_DLR_ROOT_DIR}/3rdparty/tvm/include \
   		   -I${AMBA_DLR_ROOT_DIR}/3rdparty/tvm/3rdparty/dlpack/include \
   		   -I${AMBA_DLR_ROOT_DIR}/3rdparty/tvm/3rdparty/dmlc-core/include

CXXFLAGS = $(CFLAGS) -std=c++14

LDFLAGS += \
	   -L./inc -L./src \
	   -lpthread -lm \
	   -Wl,-Bstatic -lAmbaCV_Flexidag -lambadag -Wl,-Bdynamic \
	   -L$(AMBA_DLR_ROOT_DIR)/prebuild/amba/lib -ltvm_runtime \
	   -L$(AMBA_DLR_ROOT_DIR)/prebuild/amba/lib -lamba_tvm \
	   -Wl,-rpath-link=$(FAKEROOT_DIR)/target/usr/lib \

	   #-D_GLIBCXX_USE_CXX11_ABI=1
CXXLDFLAGS = $(LDFLAGS) -lstdc++

LIB_SRCS = src/amba_tvm.c
LIB_OBJS = $(LIB_SRCS:.c=.o)

TVM_MODULE = libamba_tvm.so
TVM_VER_PREFIX = AMBA_TVM
TVM_VERSION_FILE = $(LOCAL_PATH)/inc/amba_tvm.h
TVM_SO_VER_MAJOR  := $(shell awk '/define $(TVM_VER_PREFIX)_LIB_MAJOR/{print $$3}' $(TVM_VERSION_FILE))
TVM_SO_VER_MINOR  := $(shell awk '/define $(TVM_VER_PREFIX)_LIB_MINOR/{print $$3}' $(TVM_VERSION_FILE))
TVM_SO_VER_PATCH  := $(shell awk '/define $(TVM_VER_PREFIX)_LIB_PATCH/{print $$3}' $(TVM_VERSION_FILE))
TVM_SO_VER_STRING := $(TVM_SO_VER_MAJOR).$(TVM_SO_VER_MINOR).$(TVM_SO_VER_PATCH)
TVM_SO_NAME   = $(patsubst %.so,%.so.$(TVM_SO_VER_MAJOR), $(TVM_MODULE))
TVM_SO_TARGET = $(patsubst %.so,%.so.$(TVM_SO_VER_STRING), $(TVM_MODULE))
LOCAL_SO_NAME := $(TVM_SO_NAME)

LIB_TARGET	:= $(TVM_MODULE)

all: $(LIB_TARGET)

$(LIB_TARGET): $(LIB_OBJS)
	@$(RM) $@
	$(CC) -shared -Wl,-soname,$@ -Wl,--as-needed -Wl,--warn-common -o $@  $(LIB_OBJS)

clean:
	$(RM) core $(LIB_TARGET) *.o

install: $(LIB_TARGET)
	mkdir -p $(DESTDIR)/usr/lib;
	cp -dpRf $< $(DESTDIR)/usr/lib/$(TVM_SO_TARGET)
	ln -sf $(TVM_SO_TARGET) $(DESTDIR)/usr/lib/$(TVM_SO_NAME)
	ln -sf $(TVM_SO_NAME) $(DESTDIR)/usr/lib/$(TVM_MODULE)

install_staging: $(LIB_TARGET)
	mkdir -p $(DESTDIR)/usr/lib;
	cp -dpRf $< $(DESTDIR)/usr/lib/$(TVM_SO_TARGET)
	ln -sf $(TVM_SO_TARGET) $(DESTDIR)/usr/lib/$(TVM_SO_NAME)
	ln -sf $(TVM_SO_NAME) $(DESTDIR)/usr/lib/$(TVM_MODULE)

uninstall: $(LIB_TARGET)
	for i in $(LIB_TARGET); do rm -f $(DESTDIR)/usr/lib/$$i; done

