#!/bin/bash

PUBLIC_NN_PATH=${AMBA_SDK8}/tools/ADK/public_nn/diags/file_in
SAMPLE_NN_PATH=${AMBA_SDK8}/tools/ADK/sample_nn_diag/diags/file_in
CUR_DIR=${PWD}

echo "public_nn   = ${PUBLIC_NN_PATH}"
echo "sample_nn   = ${SAMPLE_NN_PATH}"

### 1. Remove all golden data under sample_nn ###
echo "Remove golden data ...."
find ${SAMPLE_NN_PATH} -mindepth 1 -maxdepth 1 -type d | xargs -i rm -rf {}/golden

### 2. Copy golden data from public_nn ###
echo "Copy golden data ...."
cd ${SAMPLE_NN_PATH}
find . -mindepth 1 -maxdepth 1 -type d | xargs -i cp -rf ${PUBLIC_NN_PATH}/{}/golden ${SAMPLE_NN_PATH}/{}/golden
cd ${CUR_DIR}

echo "Done ...."
