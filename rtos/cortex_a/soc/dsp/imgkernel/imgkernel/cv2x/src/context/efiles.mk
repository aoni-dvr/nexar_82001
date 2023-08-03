IK_CI_CTX_FOLDER := $(IK_FOLDER)/src/context
IK_CI_CTX_SRC := $(wildcard $(IK_CI_CTX_FOLDER)/*.c)
#IK_CI_CTX_SRC += $(IK_CI_CTX_FOLDER)/context_component_if.c

IK_CI_CTX_INC := 
IK_CI_CTX_INC += -I$(IK_CI_CTX_FOLDER)/
IK_CI_CTX_INC += -I$(IK_CI_CTX_FOLDER)/../../inc/arch
IK_CI_CTX_INC += -I$(IK_CI_CTX_FOLDER)/../../inc/context
IK_CI_CTX_INC += -I$(IK_CI_CTX_FOLDER)/../../inc
IK_CI_CTX_INC += -I$(IK_CI_CTX_FOLDER)/../../inc/ikc
IK_CI_CTX_INC += -I$(IK_CI_CTX_FOLDER)/../../early_test/early_test_inc/vendors
IK_CI_CTX_INC += -I$(IK_CI_CTX_FOLDER)/../../early_test/early_test_inc/al

IK_CI_CTX_OBJ = $(IK_CI_CTX_SRC:.c=.o)