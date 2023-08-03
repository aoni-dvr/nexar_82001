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

$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/AmbaShell.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/common/AmbaShell_KeyCode.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/common/AmbaShell_CmdHistory.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/common/AmbaShell_Utility.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/common/AmbaShell_MakeDir.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_Cat.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_ChangeWorkDir.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_Checksum.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_Clear.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_CopyFile.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_DisplayMessage.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_Date.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_Echo.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_False.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_Format.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_FsThruput.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_IoStat.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_Help.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_ListDir.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_LoadBin.o
ifeq ($(CONFIG_ARM64),y)
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_MemIO_A64.o
else
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_MemIO.o
endif
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_Misc.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_MoveFile.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_PrintWorkDir.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_ProcessStatus.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_Reboot.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_RemoveDir.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_RemoveFile.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/common/AmbaShell_SaveBin.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/common/AmbaShell_Eject.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_SaveImage.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_SdShmoo.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_Sleep.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_SysInfo.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_True.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_Ver.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_Volume.o
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_Atf.o

ifeq ($(CONFIG_SOC_CV2FS),y)
ccflags-y += -I$(srctree)/vendors/ambarella/inc
$(OBJ_NAME)-objs += $(gnu_make_dir)/src/threadx/commands/AmbaShell_FuSa.o
endif
