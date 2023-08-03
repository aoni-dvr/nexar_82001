IK_CI_ARCH_FOLDER := $(IK_FOLDER)/src/arch
IK_CI_ARCH_SRC := $(wildcard $(IK_CI_ARCH_FOLDER)/*.c)

IK_CI_ARCH_INC := 
IK_CI_ARCH_INC += -I$(IK_CI_ARCH_FOLDER)/
IK_CI_ARCH_INC += -I$(IK_CI_ARCH_FOLDER)/../../inc
IK_CI_ARCH_INC += -I$(IK_CI_ARCH_FOLDER)/../../inc/ikc
IK_CI_ARCH_INC += -I$(IK_CI_ARCH_FOLDER)/../../inc/common
IK_CI_ARCH_INC += -I$(IK_CI_ARCH_FOLDER)/../../inc/arch
IK_CI_ARCH_INC += -I$(IK_CI_ARCH_FOLDER)/../../inc/config

IK_CI_ARCH_OBJ = $(IK_CI_ARCH_SRC:.c=.o)