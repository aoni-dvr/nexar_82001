ccflags-y := \
        -I$(srctree)/vendors/ambarella/inc \
        -I$(srctree)/vendors/ambarella/inc/io \
        -I$(srctree)/vendors/ambarella/inc/io/$(AMBA_CHIP_ID) \
        -I$(srctree)/vendors/ambarella/inc/dsp \
        -I$(srctree)/vendors/ambarella/inc/dsp/$(AMBA_CHIP_ID) \
        -I$(srctree)/svc/comsvc/print \
        -I$(srctree)/svc/comsvc/misc \
		-I$(srctree)/svc/comsvc/shell/inc \
        -I$(srctree)/soc/io/src/common/inc \
        -I$(srctree)/soc/io/src/common/inc/arm \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc/rtsl \
        -I$(srctree)/soc/io/src/$(AMBA_CHIP_ID)/inc

ifneq ($(OBJ_NAME),)
# Rule for libcomsvc.a
gnu_make_dir := shell

else
# Rule for libshell.a
OBJ_NAME = libshell

obj-y := $(OBJ_NAME).a
install-files += $(OBJ_NAME).a

gnu_make_dir := .

endif

$(OBJ_NAME)-objs += $(gnu_make_dir)/../../src/linux/AmbaShell.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../src/linux/commands/AmbaShell_MemIO.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../src/linux/commands/AmbaShell_Sleep.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../src/linux/commands/AmbaShell_Help.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../src/linux/commands/AmbaShell_Reboot.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../src/linux/commands/AmbaShell_DisplayMessage.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../src/linux/commands/AmbaShell_Exec.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../src/linux/commands/AmbaShell_Date.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../src/linux/commands/AmbaShell_ChangeWorkDir.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../src/linux/commands/AmbaShell_ListDir.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../src/linux/commands/AmbaShell_FsThruput.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../src/linux/commands/AmbaShell_MoveFile.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../src/linux/commands/AmbaShell_Volume.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../src/linux/commands/AmbaShell_Format.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../src/linux/commands/AmbaShell_RemoveFile.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../src/linux/commands/AmbaShell_SysInfo.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../src/common/AmbaShell_Eject.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../src/common/AmbaShell_KeyCode.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../src/common/AmbaShell_CmdHistory.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../src/common/AmbaShell_Utility.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../src/common/AmbaShell_SaveBin.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/../../src/common/AmbaShell_MakeDir.o
