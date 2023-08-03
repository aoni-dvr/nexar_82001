#!/bin/sh
curl http://tw-sdk/tw-sdk/images/target.tgz --create-dirs -o ${TARGET_DIR}.tgz
mkdir -p ${TARGET_DIR}
tar zxf ${TARGET_DIR}.tgz -C ${TARGET_DIR}/..
