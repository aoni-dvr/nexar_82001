IK_CI_EXE_FOLDER := $(IK_FOLDER)/src/executer
IK_CI_EXE_SRC := $(wildcard $(IK_CI_EXE_FOLDER)/*.c)
#IK_CI_EXE_SRC += $(IK_CI_EXE_FOLDER)/executer_component_if.c
#IK_CI_EXE_SRC += $(IK_CI_EXE_FOLDER)/executer_container_controller.c

IK_CI_EXE_INC := 
IK_CI_EXE_INC += -I$(IK_CI_EXE_FOLDER)/
IK_CI_EXE_INC += -I$(IK_CI_EXE_FOLDER)/../../inc/arch
IK_CI_EXE_INC += -I$(IK_CI_EXE_FOLDER)/../../inc/config
IK_CI_EXE_INC += -I$(IK_CI_EXE_FOLDER)/../../inc/context
IK_CI_EXE_INC += -I$(IK_CI_EXE_FOLDER)/../../inc/executer
IK_CI_EXE_INC += -I$(IK_CI_EXE_FOLDER)/../../inc/ikc
IK_CI_EXE_INC += -I$(IK_CI_EXE_FOLDER)/../../inc
IK_CI_EXE_INC += -I$(IK_CI_CTX_FOLDER)/../../early_test/early_test_inc/vendors
IK_CI_EXE_INC += -I$(IK_CI_CTX_FOLDER)/../../early_test/early_test_inc/al

IK_CI_EXE_OBJ = $(IK_CI_EXE_SRC:.c=.o)