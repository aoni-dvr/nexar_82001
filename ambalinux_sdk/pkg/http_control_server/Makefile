
export AMBALINK_CV25_DIR=${PWD}/../../output.oem/ambalink

CROSS_COMPILE=${AMBALINK_CV25_DIR}/host/bin/aarch64-none-linux-gnu-
export CC=${CROSS_COMPILE}gcc

all:
	${CC} -o sample_httpd main.c cmd_handler.c cmd_response.c md5.c cJSON/cJSON.c -lpthread -lambaipc -lsqlite3 \
	../amba_examples/Example_RtosApi/rtos_api_lib.c \
	-I../amba_examples/Example_RtosApi \
	-I${AMBALINK_CV25_DIR}/host/aarch64-buildroot-linux-gnu/sysroot/usr/include

clean:
	rm sample_httpd
