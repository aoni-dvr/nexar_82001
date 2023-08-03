#### ThreadX
ifneq ($(OBJ_NAME),)
# Rule for libcomsvc.a
gnu_make_dir := misc

else
# Rule for libmisc.a
OBJ_NAME = libmisc

obj-y := $(OBJ_NAME).a
install-files += $(OBJ_NAME).a

gnu_make_dir := .

endif

$(OBJ_NAME)-objs += $(gnu_make_dir)/AmbaMemoryLog.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/AmbaUtility.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/AmbaUtility_Crc32Hw.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/AmbaUtility_GZIPDecompress.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/AmbaDspInt.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/AmbaSvcWrap.o
