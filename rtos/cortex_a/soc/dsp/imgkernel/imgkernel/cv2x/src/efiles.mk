IK_EI_FOLDER := $(IK_FOLDER)/src
IK_EI_SRC := $(wildcard $(IK_EI_FOLDER)/*.c)

IK_EI_INC := 
IK_EI_INC += -I$(IK_EI_FOLDER)/
IK_EI_INC += -I$(IK_EI_FOLDER)/../inc
IK_EI_INC += -I$(IK_EI_FOLDER)/../inc/arch
IK_EI_INC += -I$(IK_EI_FOLDER)/../inc/config
IK_EI_INC += -I$(IK_EI_FOLDER)/../inc/context
IK_EI_INC += -I$(IK_EI_FOLDER)/../inc/executer
IK_EI_INC += -I$(IK_EI_FOLDER)/../inc/ikc

IK_EI_OBJ = $(IK_EI_SRC:.c=.o)