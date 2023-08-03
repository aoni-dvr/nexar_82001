export AMBALINK_CV25_DIR=${PWD}/../../../output.oem/ambalink

CROSS_COMPILE="${AMBALINK_CV25_DIR}/host/bin/aarch64-none-linux-gnu-"
export CC=${CROSS_COMPILE}gcc

all:
	#${CC} -o sample_rtos_api rtos_api.c rtos_api_lib.c -lambaipc -lpthread
	${CC} -o burn_info2 burn_info.c serial_port.c -lambaipc -lpthread -lrtos_api
