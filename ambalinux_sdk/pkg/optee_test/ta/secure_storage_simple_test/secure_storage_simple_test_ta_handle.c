/*
 * Copyright (C) 2018 Ambarella Inc.
 * All rights reserved.
 *
 * Author: Bo-xi Chen <bxchen@ambarella.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "tee_internal_api.h"
#include "tee_api_defines.h"
#include "trace.h"
#include "tee_api_defines_extensions.h"
#include "string.h"

#include "secure_storage_simple_test_ta_type.h"

#include "secure_storage_simple_test_ta_handle.h"

TEE_Result secure_storage_simple_test_ta_create(unsigned int paramTypes, TEE_Param params[4])
{
    TEE_Result ret = TEE_EXEC_FAIL;
    unsigned int obj_name_len = 0U;
    char *obj_name = NULL;
    UNUSED(paramTypes);
    TEE_ObjectHandle obj = NULL;

    obj_name = params[0].memref.buffer;
    obj_name_len = params[0].memref.size;

    char obj_id[obj_name_len];
    TEE_MemMove(obj_id, obj_name, obj_name_len);

    ret = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE, obj_id,
                       obj_name_len, TEE_DATA_FLAG_ACCESS_WRITE_META | TEE_DATA_FLAG_ACCESS_WRITE,
                       TEE_HANDLE_NULL , NULL, 0,
                       (&obj));
    if (TEE_SUCCESS != ret) {
        DLOG("create obj (%s) fail\n", obj_name);
        return TEE_EXEC_FAIL;
    } else {
        TEE_CloseObject(obj);
        DLOG("create obj (%s) success\n", obj_name);
        return TEE_SUCCESS;
    }
}


TEE_Result secure_storage_simple_test_ta_load(unsigned int paramTypes, TEE_Param params[4])
{
    TEE_Result ret = TEE_SUCCESS;
    char *outbuf = NULL;
    unsigned int outbufsize;
    unsigned int obj_size;
    char *obj_name = NULL;
    unsigned int obj_name_len = 0U;
    unsigned int count = 0U;
    UNUSED(paramTypes);

    TEE_ObjectHandle files_obj;
    TEE_ObjectInfo obj_info;

    unsigned int AccFlg = TEE_DATA_FLAG_ACCESS_READ |
                 TEE_DATA_FLAG_ACCESS_WRITE |
                 TEE_DATA_FLAG_ACCESS_WRITE_META |
                 TEE_DATA_FLAG_SHARE_READ |
                 TEE_DATA_FLAG_SHARE_WRITE |
                 TEE_DATA_FLAG_OVERWRITE;

    /** 1) Get the fd of secure file */
    outbuf = params[0].memref.buffer;
    outbufsize = params[0].memref.size;
    obj_name_len = params[1].memref.size;

    obj_name = TEE_Malloc(obj_name_len + 1, 0);
    TEE_MemMove(obj_name, params[1].memref.buffer, obj_name_len);

    ret = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE, obj_name,
                                   obj_name_len, AccFlg, (&files_obj));
    if (TEE_SUCCESS != ret) {
        DLOG("error: open file fail\n");
        return TEE_EXEC_FAIL;
    }

    TEE_GetObjectInfo(files_obj, &obj_info);
    obj_size = obj_info.dataSize;
    if (outbufsize < obj_size) {
        DLOG("error: output buf too small, %d, %d\n", outbufsize, obj_size);
        return TEE_EXEC_FAIL;
     }

    /** 2) Start read data from secure file */
    ret = TEE_ReadObjectData(files_obj, outbuf, obj_size, &count);
    DLOG("TEE_ReadObjectData read size, count: %d, %d\n", obj_size, count);

    TEE_CloseObject(files_obj);
    TEE_Free(obj_name);
    if (TEE_SUCCESS != ret) {
        DLOG("error: read object fail\n");
        return TEE_EXEC_FAIL;
    } else {
        return TEE_SUCCESS;
    }
}

TEE_Result secure_storage_simple_test_ta_store(unsigned int paramTypes, TEE_Param params[4])
{
    TEE_Result ret = TEE_SUCCESS;
    char* inbuf = NULL;
    unsigned int writelen = 0U;
    char *obj_name = NULL;
    unsigned int obj_name_len = 0U;
    UNUSED(paramTypes);

    TEE_ObjectHandle files_obj;
    unsigned int AccFlg = TEE_DATA_FLAG_ACCESS_READ |
                 TEE_DATA_FLAG_ACCESS_WRITE |
                 TEE_DATA_FLAG_ACCESS_WRITE_META |
                 TEE_DATA_FLAG_SHARE_READ |
                 TEE_DATA_FLAG_SHARE_WRITE |
                 TEE_DATA_FLAG_OVERWRITE;

    /** 1) Get the fd of secure file */
    inbuf = params[0].memref.buffer;
    writelen = params[0].memref.size;
    obj_name_len = params[1].memref.size;

    obj_name = TEE_Malloc(obj_name_len + 1, 0);
    TEE_MemMove(obj_name, params[1].memref.buffer, obj_name_len);

    ret = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE, obj_name,
                                   obj_name_len, AccFlg, (&files_obj));
    if (TEE_SUCCESS != ret) {
        DLOG("error: open file fail\n");
        return TEE_EXEC_FAIL;
    }

    /** 2) Start write data to secure file */
    ret = TEE_WriteObjectData(files_obj, inbuf, writelen);

    TEE_CloseObject(files_obj);
    TEE_Free(obj_name);
    if (TEE_SUCCESS != ret) {
        DLOG("error: wtire file fail\n");
        return TEE_EXEC_FAIL;
    } else {
        DLOG("write obj %s success\n", obj_name);
        return TEE_SUCCESS;
    }
}

TEE_Result secure_storage_simple_test_ta_load_2buffer(unsigned int paramTypes, TEE_Param params[4])
{
    TEE_Result ret = TEE_SUCCESS;
    char *outbuf_1 = NULL;
    //unsigned int outbuf_1_size;
    char *outbuf_2 = NULL;
    //unsigned int outbuf_2_size;
    unsigned int obj_size;
    char *obj_name = NULL;
    unsigned int obj_name_len = 0U;
    unsigned int count = 0U;
    UNUSED(paramTypes);

    TEE_ObjectHandle files_obj;
    TEE_ObjectInfo obj_info;

    unsigned int AccFlg = TEE_DATA_FLAG_ACCESS_READ |
                 TEE_DATA_FLAG_ACCESS_WRITE |
                 TEE_DATA_FLAG_ACCESS_WRITE_META |
                 TEE_DATA_FLAG_SHARE_READ |
                 TEE_DATA_FLAG_SHARE_WRITE |
                 TEE_DATA_FLAG_OVERWRITE;

    /** 1) Get the fd of secure file */
    outbuf_1 = params[0].memref.buffer;
    //outbuf_1_size = params[0].memref.size;
    outbuf_2 = params[1].memref.buffer;
    //outbuf_2_size = params[1].memref.size;
    obj_name_len = params[2].memref.size;

    obj_name = TEE_Malloc(obj_name_len + 1, 0);
    TEE_MemMove(obj_name, params[2].memref.buffer, obj_name_len);

    ret = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE, obj_name,
                                   obj_name_len, AccFlg, (&files_obj));
    if (TEE_SUCCESS != ret) {
        DLOG("error: open file fail\n");
        return TEE_EXEC_FAIL;
    }

    TEE_GetObjectInfo(files_obj, &obj_info);
    obj_size = obj_info.dataSize;


    /** 2) Start read data from secure file */
    ret = TEE_ReadObjectData(files_obj, outbuf_1, 20, &count);
    if (TEE_SUCCESS != ret) {
        DLOG("error: read object fail\n");
        return TEE_EXEC_FAIL;
    }
    DLOG("TEE_ReadObjectData step1 count: %d\n", count);

    ret = TEE_ReadObjectData(files_obj, outbuf_2, obj_size, &count);
    if (TEE_SUCCESS != ret) {
        DLOG("error: read object fail\n");
        return TEE_EXEC_FAIL;
    }
    DLOG("TEE_ReadObjectData step2 count: %d\n", count);

    TEE_CloseObject(files_obj);
    TEE_Free(obj_name);
    return TEE_SUCCESS;

}


TEE_Result secure_storage_simple_test_ta_delete(unsigned int paramTypes, TEE_Param params[4])
{
    TEE_Result ret = TEE_SUCCESS;
    char* oldname = NULL;
    unsigned int oldlen = 0U;

    UNUSED(paramTypes);
    TEE_ObjectHandle files_obj;
    unsigned int AccFlg = TEE_DATA_FLAG_ACCESS_READ |
                 TEE_DATA_FLAG_ACCESS_WRITE |
                 TEE_DATA_FLAG_ACCESS_WRITE_META |
                 TEE_DATA_FLAG_SHARE_READ |
                 TEE_DATA_FLAG_SHARE_WRITE |
                 TEE_DATA_FLAG_OVERWRITE;

    /** 1) Get the fd vaule of file which need be close */
    oldlen = params[0].memref.size;
    oldname = TEE_Malloc(oldlen, 0);
    TEE_MemMove(oldname, params[0].memref.buffer, oldlen);

    ret = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE, oldname,
                                   oldlen, AccFlg, (&files_obj));
    if (TEE_SUCCESS != ret) {
        DLOG("error: open file fail\n");
        return TEE_EXEC_FAIL;
    }

    /** 2) Close the object handle of secure file */
    TEE_CloseAndDeletePersistentObject(files_obj);

    TEE_Free(oldname);
    if (TEE_SUCCESS != ret) {
        DLOG("error: delete file fail\n");
        return TEE_EXEC_FAIL;
    } else {
        return TEE_SUCCESS;
    }
}
