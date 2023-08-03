IK_CI_CFG_FOLDER := $(IK_FOLDER)/src/config
IK_CI_CFG_SRC := $(wildcard $(IK_CI_CFG_FOLDER)/*.c)

IK_CI_CFG_INC := 
IK_CI_CFG_INC += -I$(IK_CI_CFG_FOLDER)/
IK_CI_CFG_INC += -I$(IK_CI_CFG_FOLDER)/../../inc
IK_CI_CFG_INC += -I$(IK_CI_CFG_FOLDER)/../../inc/common
IK_CI_CFG_INC += -I$(IK_CI_CFG_FOLDER)/../../inc/arch
IK_CI_CFG_INC += -I$(IK_CI_CFG_FOLDER)/../../inc/context
IK_CI_CFG_INC += -I$(IK_CI_CFG_FOLDER)/../../inc/config
IK_CI_CFG_INC += -I$(IK_CI_CFG_FOLDER)/../../inc/ikc
IK_CI_CFG_INC += -I$(IK_CI_CFG_FOLDER)/../../inc/iks
IK_CI_CFG_INC += -I$(IK_CI_CFG_FOLDER)/../../../../imgkernelcore/cv2xfs/early_test/early_test_inc/al #for coverity

IK_CI_CFG_OBJ = $(IK_CI_CFG_SRC:.c=.o)
