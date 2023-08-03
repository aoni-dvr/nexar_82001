ifneq ($(OBJ_NAME),)
# Rule for libcomsvc.a
gnu_make_dir := print

else
# Rule for libprint.a
OBJ_NAME = libprint

obj-y := $(OBJ_NAME).a
install-files += $(OBJ_NAME).a

gnu_make_dir := .

endif

$(OBJ_NAME)-objs += $(gnu_make_dir)/AmbaPrint.o
