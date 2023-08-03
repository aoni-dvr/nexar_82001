host-output-files += host_crc32

hostprogs-y	:= $(host-output-files)

always		:= $(hostprogs-y)

HOST_EXTRACFLAGS += -m32

install-files := $(host-output-files)
remove-installed-files := $(host-output-files)

ifeq ($(HOST_OS), CYGWIN)
install-path  := $(call cygpath,$(srctree))/svc/comsvc/ambalink/tools/exec/win
clean-files += host_crc32.exe
else
install-path  := $(srctree)/svc/comsvc/ambalink/tools/exec/lnx
clean-files += host_crc32
endif
