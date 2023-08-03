/**
 *  @file loader.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Loader APIs
 *
 */

#include "os_api.h"
#include "schdr.h"
#include "dram_mmap.h"
#include "schdr_util.h"
#include "ambacv_kal.h"
#include "cvsched_flexibin_format.h"

#if !defined(CONFIG_BUILD_CV_THREADX)
static uint32_t Crc32Table[] = {
    0x00000000U, 0x77073096U, 0xee0e612cU, 0x990951baU, 0x076dc419U, 0x706af48fU,
    0xe963a535U, 0x9e6495a3U, 0x0edb8832U, 0x79dcb8a4U, 0xe0d5e91eU, 0x97d2d988U,
    0x09b64c2bU, 0x7eb17cbdU, 0xe7b82d07U, 0x90bf1d91U, 0x1db71064U, 0x6ab020f2U,
    0xf3b97148U, 0x84be41deU, 0x1adad47dU, 0x6ddde4ebU, 0xf4d4b551U, 0x83d385c7U,
    0x136c9856U, 0x646ba8c0U, 0xfd62f97aU, 0x8a65c9ecU, 0x14015c4fU, 0x63066cd9U,
    0xfa0f3d63U, 0x8d080df5U, 0x3b6e20c8U, 0x4c69105eU, 0xd56041e4U, 0xa2677172U,
    0x3c03e4d1U, 0x4b04d447U, 0xd20d85fdU, 0xa50ab56bU, 0x35b5a8faU, 0x42b2986cU,
    0xdbbbc9d6U, 0xacbcf940U, 0x32d86ce3U, 0x45df5c75U, 0xdcd60dcfU, 0xabd13d59U,
    0x26d930acU, 0x51de003aU, 0xc8d75180U, 0xbfd06116U, 0x21b4f4b5U, 0x56b3c423U,
    0xcfba9599U, 0xb8bda50fU, 0x2802b89eU, 0x5f058808U, 0xc60cd9b2U, 0xb10be924U,
    0x2f6f7c87U, 0x58684c11U, 0xc1611dabU, 0xb6662d3dU, 0x76dc4190U, 0x01db7106U,
    0x98d220bcU, 0xefd5102aU, 0x71b18589U, 0x06b6b51fU, 0x9fbfe4a5U, 0xe8b8d433U,
    0x7807c9a2U, 0x0f00f934U, 0x9609a88eU, 0xe10e9818U, 0x7f6a0dbbU, 0x086d3d2dU,
    0x91646c97U, 0xe6635c01U, 0x6b6b51f4U, 0x1c6c6162U, 0x856530d8U, 0xf262004eU,
    0x6c0695edU, 0x1b01a57bU, 0x8208f4c1U, 0xf50fc457U, 0x65b0d9c6U, 0x12b7e950U,
    0x8bbeb8eaU, 0xfcb9887cU, 0x62dd1ddfU, 0x15da2d49U, 0x8cd37cf3U, 0xfbd44c65U,
    0x4db26158U, 0x3ab551ceU, 0xa3bc0074U, 0xd4bb30e2U, 0x4adfa541U, 0x3dd895d7U,
    0xa4d1c46dU, 0xd3d6f4fbU, 0x4369e96aU, 0x346ed9fcU, 0xad678846U, 0xda60b8d0U,
    0x44042d73U, 0x33031de5U, 0xaa0a4c5fU, 0xdd0d7cc9U, 0x5005713cU, 0x270241aaU,
    0xbe0b1010U, 0xc90c2086U, 0x5768b525U, 0x206f85b3U, 0xb966d409U, 0xce61e49fU,
    0x5edef90eU, 0x29d9c998U, 0xb0d09822U, 0xc7d7a8b4U, 0x59b33d17U, 0x2eb40d81U,
    0xb7bd5c3bU, 0xc0ba6cadU, 0xedb88320U, 0x9abfb3b6U, 0x03b6e20cU, 0x74b1d29aU,
    0xead54739U, 0x9dd277afU, 0x04db2615U, 0x73dc1683U, 0xe3630b12U, 0x94643b84U,
    0x0d6d6a3eU, 0x7a6a5aa8U, 0xe40ecf0bU, 0x9309ff9dU, 0x0a00ae27U, 0x7d079eb1U,
    0xf00f9344U, 0x8708a3d2U, 0x1e01f268U, 0x6906c2feU, 0xf762575dU, 0x806567cbU,
    0x196c3671U, 0x6e6b06e7U, 0xfed41b76U, 0x89d32be0U, 0x10da7a5aU, 0x67dd4accU,
    0xf9b9df6fU, 0x8ebeeff9U, 0x17b7be43U, 0x60b08ed5U, 0xd6d6a3e8U, 0xa1d1937eU,
    0x38d8c2c4U, 0x4fdff252U, 0xd1bb67f1U, 0xa6bc5767U, 0x3fb506ddU, 0x48b2364bU,
    0xd80d2bdaU, 0xaf0a1b4cU, 0x36034af6U, 0x41047a60U, 0xdf60efc3U, 0xa867df55U,
    0x316e8eefU, 0x4669be79U, 0xcb61b38cU, 0xbc66831aU, 0x256fd2a0U, 0x5268e236U,
    0xcc0c7795U, 0xbb0b4703U, 0x220216b9U, 0x5505262fU, 0xc5ba3bbeU, 0xb2bd0b28U,
    0x2bb45a92U, 0x5cb36a04U, 0xc2d7ffa7U, 0xb5d0cf31U, 0x2cd99e8bU, 0x5bdeae1dU,
    0x9b64c2b0U, 0xec63f226U, 0x756aa39cU, 0x026d930aU, 0x9c0906a9U, 0xeb0e363fU,
    0x72076785U, 0x05005713U, 0x95bf4a82U, 0xe2b87a14U, 0x7bb12baeU, 0x0cb61b38U,
    0x92d28e9bU, 0xe5d5be0dU, 0x7cdcefb7U, 0x0bdbdf21U, 0x86d3d2d4U, 0xf1d4e242U,
    0x68ddb3f8U, 0x1fda836eU, 0x81be16cdU, 0xf6b9265bU, 0x6fb077e1U, 0x18b74777U,
    0x88085ae6U, 0xff0f6a70U, 0x66063bcaU, 0x11010b5cU, 0x8f659effU, 0xf862ae69U,
    0x616bffd3U, 0x166ccf45U, 0xa00ae278U, 0xd70dd2eeU, 0x4e048354U, 0x3903b3c2U,
    0xa7672661U, 0xd06016f7U, 0x4969474dU, 0x3e6e77dbU, 0xaed16a4aU, 0xd9d65adcU,
    0x40df0b66U, 0x37d83bf0U, 0xa9bcae53U, 0xdebb9ec5U, 0x47b2cf7fU, 0x30b5ffe9U,
    0xbdbdf21cU, 0xcabac28aU, 0x53b39330U, 0x24b4a3a6U, 0xbad03605U, 0xcdd70693U,
    0x54de5729U, 0x23d967bfU, 0xb3667a2eU, 0xc4614ab8U, 0x5d681b02U, 0x2a6f2b94U,
    0xb40bbe37U, 0xc30c8ea1U, 0x5a05df1bU, 0x2d02ef8dU
};


static uint32_t check_crc32(const uint8_t *pBuffer, uint32_t Size)
{
    const uint8_t *p;
    uint32_t Crc;
    uint32_t idx;

    p = pBuffer;
    Crc = ~0U;

    while (Size > 0U) {
        idx = (Crc ^ (uint32_t)*p) & 0x0FFU;
        Crc = Crc32Table[idx] ^ (Crc >> 8U);
        p++;
        Size--;
    }

    return Crc ^ ~0U;
}
#else
#define check_crc32   AmbaUtility_Crc32
#endif

#if defined(AMBACV_KERNEL_SUPPORT) || defined(RTOS_BUILD)
static uint32_t copy_visorc_expand(const void* ptr, uint64_t size, uint64_t real_size)
{
    void *ptr_void;
    const uint8_t *ptr_u8;
    uint64_t ptr_u64 = 0UL;
    uint32_t  retval = ERRCODE_NONE;
    uint32_t visorc_crc32;
    SCHDR_VISORC_POST_HEADER_s visorc_expand = {0};

    typecast(&ptr_u64, &ptr);
    ptr_u64 = ptr_u64 + real_size - (uint64_t)sizeof(visorc_expand);
    typecast(&ptr_void, &ptr_u64);
    retval = thread_memcpy(&visorc_expand, ptr_void, sizeof(visorc_expand));
    if (retval == ERRCODE_NONE) {
        if (visorc_expand.magic1 == SCHDR_VISORC_CRC) {
            typecast(&ptr_u8, &ptr);
            visorc_crc32 = check_crc32(ptr_u8, visorc_expand.size);
            if (visorc_expand.crc32 != visorc_crc32) {
                console_printU5("[ERROR] copy_visorc_expand(): check failed: visorc expected crc32=0x%x, got 0x%x\n", visorc_expand.crc32, visorc_crc32, 0U, 0U, 0U);
                retval = ERR_INTF_SCHDR_VISORC_BIN_CRC_MISMATCHED;
            } else {
                typecast(&ptr_u64, &ptr);
                ptr_u64 = ptr_u64 + size - (uint64_t)sizeof(visorc_expand);
                typecast(&ptr_void, &ptr_u64);
                retval = thread_memcpy(ptr_void, &visorc_expand, sizeof(visorc_expand));
            }
        }
    }

    return retval;
}


static uint32_t load_binary_ext(const char *file_name, void* ptr, uint32_t size, uint32_t *real_size)
{
    THREAD_FILE *ifp;
    uint64_t size64 = 0UL;
    uint32_t  retval = ERRCODE_NONE;

    if (file_name == NULL) {
        console_printU5("[ERROR] load_binary_ext() : path == NULL !\n", 0U, 0U, 0U, 0U, 0U);
        retval = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        ifp = thread_fopen(file_name, "rb");
        if (ifp == NULL) {
            console_printS5("[ERROR] load_binary_ext() : Can't open input file %s !\n", file_name, NULL, NULL, NULL, NULL);
            retval = ERR_DRV_SCHDR_FILE_DESC_INVALID;
        } else {
            retval = thread_fseek(ifp, 0, THREAD_SEEK_END);
            if(retval == ERRCODE_NONE) {
                size64 = thread_ftell(ifp);
                retval = thread_fseek(ifp, 0, THREAD_SEEK_SET);
            }

            if(retval == ERRCODE_NONE) {
                if (size64 == 0U) {
                    console_printS5("[ERROR] load_binary_ext() : zero-sized file %s !\n", file_name, NULL, NULL, NULL, NULL);
                    retval = ERR_DRV_SCHDR_VISORC_BIN_SIZE_UNAVAILABLE;
                } else if (size64 > size) {
                    console_printS5("[ERROR] load_binary_ext() : size too big file %s !\n", file_name, NULL, NULL, NULL, NULL);
                    retval = ERR_DRV_SCHDR_VISORC_BIN_SIZE_OUT_OF_RANGE;
                } else {
                    if (size64 != thread_fread(ptr, 1, (uint32_t)size64, ifp)) {
                        console_printS5("[ERROR] load_binary_ext() : Fail to read file %s !\n", file_name, NULL, NULL, NULL, NULL);
                        retval = ERR_DRV_SCHDR_FILE_READ_FAIL;
                    }
                    *real_size = (uint32_t)size64;
                }
            }
            if( thread_fclose(ifp) != ERRCODE_NONE) {
                console_printU5("[ERROR] load_binary_ext() : thread_fclose fail ", 0U, 0U, 0U, 0U, 0U);
            }
        }
    }
    return retval;
}

static uint32_t load_binary(int32_t fd, const char *path, ambacv_mem_region_t *region)
{
    THREAD_FILE *ifp;
    void *mmap_ptr;
    void *ptr;
    uint64_t size = 0UL,mmap_size = 0UL;
    uint32_t  retval = ERRCODE_NONE;

    if (path == NULL) {
        console_printU5("[ERROR] load_binary() : path == NULL !\n", 0U, 0U, 0U, 0U, 0U);
        retval = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        ifp = thread_fopen(path, "rb");
        if (ifp == NULL) {
            console_printS5("[ERROR] load_binary() : Can't open input file %s !\n", path, NULL, NULL, NULL, NULL);
            retval = ERR_DRV_SCHDR_FILE_DESC_INVALID;
        } else {
            retval = thread_fseek(ifp, 0, THREAD_SEEK_END);
            if(retval == ERRCODE_NONE) {
                size = thread_ftell(ifp);
                retval = thread_fseek(ifp, 0, THREAD_SEEK_SET);
            }

            if(retval == ERRCODE_NONE) {
                if (size == 0U) {
                    console_printS5("[ERROR] load_binary() : zero-sized file %s !\n", path, NULL, NULL, NULL, NULL);
                    region->size = 0;
                    retval = ERR_DRV_SCHDR_VISORC_BIN_SIZE_UNAVAILABLE;
                } else if (size > region->size) {
                    console_printS5("[ERROR] load_binary() : size too big file %s !\n", path, NULL, NULL, NULL, NULL);
                    retval = ERR_DRV_SCHDR_VISORC_BIN_SIZE_OUT_OF_RANGE;
                } else {
                    mmap_size = region->size;
                    mmap_ptr = ambacv_mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED,
                                           fd, ambacv_c2p(region->base));
                    ptr = mmap_ptr;
                    if (mmap_ptr == MAP_FAILED) {
                        console_printU5("[ERROR] load_binary() : Failed to mmap Pa 0x%llx Size 0x%llx !\n", region->base, region->size, 0U, 0U, 0U);
                        retval = ERR_DRV_SCHDR_BIN_MAP_FAIL;
                    } else {
                        retval = thread_memset(ptr, 0, (size_t)region->size);
                    }

                    if(retval == ERRCODE_NONE) {
                        if (size != thread_fread(ptr, 1, (uint32_t)size, ifp)) {
                            console_printS5("[ERROR] load_binary() : Fail to read file %s !\n", path, NULL, NULL, NULL, NULL);
                            retval = ERR_DRV_SCHDR_FILE_READ_FAIL;
                        } else {
                            retval = copy_visorc_expand(ptr, region->size, size);
                        }

                        if(ambacv_munmap(mmap_ptr, mmap_size) != ERRCODE_NONE) {
                            console_printU5("[ERROR] load_binary() : ambacv_munmap fail", 0U, 0U, 0U, 0U, 0U);
                        }
                    }
                }
            }
            if( thread_fclose(ifp) != ERRCODE_NONE) {
                console_printU5("[ERROR] load_binary() : thread_fclose fail ", 0U, 0U, 0U, 0U, 0U);
            }
        }
    }
    return retval;
}

int32_t schdr_load_binary(const char *path)
{
    char file_name[256];
    ambacv_mem_t mem = {0};
    const ambacv_mem_t *pmem = &mem;
    uint32_t  retval = ERRCODE_NONE;
    int32_t fd = -1;
    uint32_t numwritten,tmp_numwritten;
    void *ptr;
    ambacv_log_t bin_path;

    if (schdr_sys_state.state != SCHDR_STATE_ACTIVE) {
        fd = ambacv_open("/dev/ambacv", O_SYNC | O_RDWR);
        if (fd < 0) {
            console_printU5("[ERROR] schdr_load_binary() : Can't open /dev/ambacv !\n", 0U, 0U, 0U, 0U, 0U);
        } else {
            /*  Get ATT info    */
            retval = ambacv_att_init();
            if(retval == ERRCODE_NONE) {
                typecast(&ptr,&pmem);
                retval = ambacv_ioctl(fd, AMBACV_GET_MEM_LAYOUT, ptr);
                if (retval != ERRCODE_NONE) {
                    console_printU5("[ERROR] schdr_load_binary() : Can't get memory layout from kernel !\n", 0U, 0U, 0U, 0U, 0U);
                } else {
                    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "ambacv visorc binary layout:\n", 0U, 0U, 0U, 0U, 0U);
                    if (mem.vp.size != 0U) {
                        numwritten = 0U;
                        tmp_numwritten = sprintf_str(&file_name[numwritten], path);
                        numwritten += tmp_numwritten;
#if defined(CONFIG_BUILD_CV_THREADX)
                        tmp_numwritten = sprintf_str(&file_name[numwritten], "\\orcvp.bin");
                        numwritten += tmp_numwritten;
#else
                        tmp_numwritten = sprintf_str(&file_name[numwritten], "/orcvp.bin");
                        numwritten += tmp_numwritten;
#endif
                        retval = load_binary(fd, file_name, &mem.vp);
                        if (retval != ERRCODE_NONE) {
                            console_printS5("[ERROR] schdr_load_binary() : load %s fail !\n", file_name, NULL, NULL, NULL, NULL);
                        } else {
                            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "VP:   [0x%llx ++ 0x%llx]\n",ambacv_c2p(mem.vp.base), mem.vp.size, 0U, 0U, 0U);
                        }
                    }

                    if (mem.sod.size != 0U) {
                        numwritten = 0U;
                        tmp_numwritten = sprintf_str(&file_name[numwritten], path);
                        numwritten += tmp_numwritten;
#if defined(CONFIG_BUILD_CV_THREADX)
                        tmp_numwritten  = sprintf_str(&file_name[numwritten], "\\orcsod.bin");
                        numwritten += tmp_numwritten;
#else
                        tmp_numwritten = sprintf_str(&file_name[numwritten], "/orcsod.bin");
                        numwritten += tmp_numwritten;
#endif
                        (void) numwritten;
                        retval = load_binary(fd, file_name, &mem.sod);
                        if (retval != ERRCODE_NONE) {
                            console_printS5("[ERROR] schdr_load_binary() : load %s fail !\n", file_name, NULL, NULL, NULL, NULL);
                        } else {
                            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "SOD:   [0x%llx ++ 0x%llx]\n",ambacv_c2p(mem.sod.base), mem.sod.size, 0U, 0U, 0U);
                        }
                    }

                    if (retval == ERRCODE_NONE) {
                        retval = thread_memcpy(bin_path.binary_path,path,256);
                        if(retval == ERRCODE_NONE) {
                            retval = ambacv_ioctl(fd, AMBACV_SET_BIN_PATH, &bin_path);
                            if (retval != 0U) {
                                console_printU5("[ERROR] schdr_load_binary() : AMBACV_SET_BIN_PATH ioctl failed! !\n", 0U, 0U, 0U, 0U, 0U);
                            }
                        }
                    }
                    if (retval == ERRCODE_NONE) {
                        typecast(&ptr,&pmem);
                        retval = ambacv_ioctl(fd, AMBACV_UCODE_LOADED, ptr);
                        if (retval != ERRCODE_NONE) {
                            console_printU5("[ERROR] schdr_load_binary() : AMBCV_UCODE_LOADED ioctl failed! !\n", 0U, 0U, 0U, 0U, 0U);
                        }
                    }
                }
            }
            if(ambacv_close(fd) != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_load_binary() : ambacv_close failed! !\n", 0U, 0U, 0U, 0U, 0U);
            }
        }
    } else {
        retval = ERR_DRV_SCHDR_ALREADY_ACTIVATE;
        console_printU5("[ERROR] schdr_load_binary() : scheduler already active \n", 0U, 0U, 0U, 0U, 0U);
    }
    return (int32_t)retval;
}


int32_t schdr_load_binary_ext(const char *file_name, uint32_t bin_type, schdr_load_cb load_cb)
{
    ambacv_mem_t mem = {0};
    const ambacv_mem_t *pmem = &mem;
    uint32_t retval = ERRCODE_NONE;
    int32_t fd = -1;
    void *mmap_ptr;
    void *ptr;
    uint64_t pa = 0UL,size = 0UL, mmap_size = 0UL;
    uint32_t real_size = 0U;

    if (schdr_sys_state.state != SCHDR_STATE_ACTIVE) {
        fd = ambacv_open("/dev/ambacv", O_SYNC | O_RDWR);
        if (fd < 0) {
            console_printU5("[ERROR] schdr_load_binary_ext() : Can't open /dev/ambacv !\n", 0U, 0U, 0U, 0U, 0U);
        } else {
            /*  Get ATT info    */
            retval = ambacv_att_init();
            if(retval == ERRCODE_NONE) {
                typecast(&ptr,&pmem);
                retval = ambacv_ioctl(fd, AMBACV_GET_MEM_LAYOUT, ptr);
                if (retval != ERRCODE_NONE) {
                    console_printU5("[ERROR] schdr_load_binary_ext() : Can't get memory layout from kernel !\n", 0U, 0U, 0U, 0U, 0U);
                } else {
                    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "ambacv visorc binary layout:\n", 0U, 0U, 0U, 0U, 0U);
                    if (bin_type == SCHDR_VP_BIN) {
                        module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "VP:   [0x%llx ++ 0x%llx]\n",ambacv_c2p(mem.vp.base), mem.vp.size, 0U, 0U, 0U);
                        pa = ambacv_c2p(mem.vp.base);
                        size =  mem.vp.size;
                    } else {
                        module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "SOD:   [0x%llx ++ 0x%llx]\n",ambacv_c2p(mem.sod.base), mem.sod.size, 0U, 0U, 0U);
                        pa = ambacv_c2p(mem.sod.base);
                        size =  mem.sod.size;
                    }
                    mmap_size = size;
                    mmap_ptr = ambacv_mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED,
                                           fd, pa);
                    ptr = mmap_ptr;
                    if (mmap_ptr == MAP_FAILED) {
                        console_printU5("[ERROR] schdr_load_binary_ext() : Failed to mmap for pa 0x%llx size 0x%llx !\n", pa, size, 0U, 0U, 0U);
                        retval = ERR_DRV_SCHDR_BIN_MAP_FAIL;
                    } else {
                        retval = thread_memset(ptr, 0, (size_t)size);
                        if (retval != ERRCODE_NONE) {
                            console_printU5("[ERROR] schdr_load_binary_ext() : thread_memset fail ret=0x%x\n", retval, 0U, 0U, 0U, 0U);
                        } else {
                            if(load_cb != NULL) {
                                retval = load_cb(file_name, ptr, (uint32_t)size, &real_size);
                            } else {
                                retval = load_binary_ext(file_name, ptr, (uint32_t)size, &real_size);
                            }
                        }

                        if (retval != ERRCODE_NONE) {
                            console_printS5("[ERROR] schdr_load_binary_ext() : load %s fail !\n", file_name, NULL, NULL, NULL, NULL);
                            retval = ERR_DRV_SCHDR_VISORC_LOAD_FAIL;
                        } else {
                            retval = copy_visorc_expand(ptr, size, (uint64_t)real_size);
                            if (retval != ERRCODE_NONE) {
                                console_printU5("[ERROR] schdr_load_binary_ext() : copy_visorc_expand fail ret=0x%x\n", retval, 0U, 0U, 0U, 0U);
                            } else {
                                typecast(&ptr,&pmem);
                                retval = ambacv_ioctl(fd, AMBACV_UCODE_LOADED, ptr);
                            }
                        }

                        if( ambacv_munmap(mmap_ptr, mmap_size) != ERRCODE_NONE) {
                            console_printU5("[ERROR] schdr_load_binary_ext(): ambacv_munmap fail", 0U, 0U, 0U, 0U, 0U);
                        }
                    }
                }
            }
            if(ambacv_close(fd) != ERRCODE_NONE) {
                console_printU5("[ERROR] schdr_load_binary_ext() : ambacv_close failed! !\n", 0U, 0U, 0U, 0U, 0U);
            }
        }
    } else {
        retval = ERR_DRV_SCHDR_ALREADY_ACTIVATE;
        console_printU5("[ERROR] schdr_load_binary_ext() : scheduler already active \n", 0U, 0U, 0U, 0U, 0U);
    }
    return (int32_t)retval;
}

uint32_t schdr_check_visorc(uint32_t bin_type)
{
    ambacv_mem_t mem = {0};
    int32_t fd = -1;
    uint32_t retval = ERRCODE_NONE;
    uint64_t visorc_size = 0UL;
    uint32_t visorc_crc32;
    const void *visorc_mem;
    uint64_t visorc_mem_u64 = 0UL;
    const uint8_t *visorc_mem_u8;
    const SCHDR_VISORC_POST_HEADER_s *visorc_expand;

    fd = ambacv_open("/dev/ambacv", O_SYNC | O_RDWR);
    if (fd < 0) {
        console_printU5("[ERROR] schdr_check_visorc(): Can't open /dev/ambacv !\n", 0U, 0U, 0U, 0U, 0U);
        retval = ERR_DRV_SCHDR_DEV_UNAVAILABLE;
    } else {
        /*  Get ATT info    */
        retval = ambacv_att_init();

        if (retval == ERRCODE_NONE) {
            retval = ambacv_ioctl(fd, AMBACV_GET_MEM_LAYOUT, &mem);
        }

        if (retval == ERRCODE_NONE) {
            if (bin_type == SCHDR_VP_BIN) {
                visorc_size = mem.vp.size - (uint64_t)sizeof(SCHDR_VISORC_POST_HEADER_s);
                visorc_mem = ambacv_mmap(NULL, mem.vp.size, PROT_READ | PROT_WRITE, MAP_SHARED,
                                         fd, ambacv_c2p(mem.vp.base));

                if (visorc_mem == MAP_FAILED) {
                    console_printU5("[ERROR] schdr_check_visorc(): Failed to mmap! Pa 0x%llx Size 0x%llx \n", mem.vp.base, mem.vp.size, 0U, 0U, 0U);
                    retval = ERR_DRV_SCHDR_BIN_MAP_FAIL;
                }
            } else {
                visorc_size = mem.sod.size - (uint64_t)sizeof(SCHDR_VISORC_POST_HEADER_s);
                visorc_mem = ambacv_mmap(NULL, mem.sod.size, PROT_READ | PROT_WRITE, MAP_SHARED,
                                         fd, ambacv_c2p(mem.sod.base));

                if (visorc_mem == MAP_FAILED) {
                    console_printU5("[ERROR] schdr_check_visorc(): Failed to mmap! Pa 0x%llx Size 0x%llx \n", mem.vp.base, mem.vp.size, 0U, 0U, 0U);
                    retval = ERR_DRV_SCHDR_BIN_MAP_FAIL;
                }
            }

            if (retval == ERRCODE_NONE) {
                typecast(&visorc_mem_u64,&visorc_mem);
                typecast(&visorc_mem_u8,&visorc_mem);
                visorc_mem_u64 = visorc_mem_u64 + visorc_size;
                typecast(&visorc_expand,&visorc_mem_u64);
                if (visorc_expand->magic1 != SCHDR_VISORC_CRC) {
                    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"[INFO] schdr_check_visorc(): There is no CRC in this visorc, skip CRC check.\n", 0U, 0U, 0U, 0U, 0U);
                } else {
                    visorc_crc32 = check_crc32(visorc_mem_u8, visorc_expand->size);
                    if (visorc_expand->crc32 != visorc_crc32) {
                        console_printU5("[ERROR] schdr_check_visorc(): check failed: visorc expected crc32=0x%x, got 0x%x\n", visorc_expand->crc32, visorc_crc32, 0U, 0U, 0U);
                        retval = ERR_INTF_SCHDR_VISORC_BIN_CRC_MISMATCHED;
                    }
                }
                if (bin_type == SCHDR_VP_BIN) {
                    if( ambacv_munmap(visorc_mem, mem.vp.size) != ERRCODE_NONE) {
                        console_printU5("[ERROR] schdr_check_visorc(): ambacv_munmap fail", 0U, 0U, 0U, 0U, 0U);
                    }
                } else {
                    if( ambacv_munmap(visorc_mem, mem.sod.size) != ERRCODE_NONE) {
                        console_printU5("[ERROR] schdr_check_visorc(): ambacv_munmap fail", 0U, 0U, 0U, 0U, 0U);
                    }
                }
            }
        }
        if(ambacv_close(fd) != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_check_visorc() : ambacv_close fail", 0U, 0U, 0U, 0U, 0U);
        }
    }

    return retval;
}
#endif

uint32_t schdr_check_flexibin(const char *pBuffer, uint32_t buffer_size)
{
    uint32_t retval = ERRCODE_NONE;
    uint32_t i;
    uint32_t num_components;
    uint32_t supported_crc32 = 0U;
    uint32_t pre_crc32;
    uint32_t flexibin_crc32;
    uint64_t pBuffer_u64 = 0UL;
    const uint8_t *pBuffer_u8;
    flexibin_main_header_t *component;


    typecast(&component,&pBuffer);
    if (component->total_filesize > buffer_size) {
        console_printU5("[ERROR] schdr_check_flexibin(): check failed: total_filesize(0x%x) > buffer_size(0x%x)\n", component->total_filesize, buffer_size, 0U, 0U, 0U);
        retval = ERR_INTF_FLEXIDAG_BIN_CRC_MISMATCHED;
    } else {
        /* Temporarily set component->hdr.reserved_crc32 to 0 for calculate total crc32) */
        typecast(&pBuffer_u8,&pBuffer);
        pre_crc32 = component->hdr.reserved_crc32;
        component->hdr.reserved_crc32 = 0U;
        flexibin_crc32 = check_crc32(pBuffer_u8, component->total_filesize);
        component->hdr.reserved_crc32 = pre_crc32;
        if (component->hdr.reserved_crc32 == 0U) {
            console_printU5("[INFO] schdr_check_flexibin(): There is no CRC in this flexibin, skip CRC check.\n", 0U, 0U, 0U, 0U, 0U);
        } else if (component->hdr.reserved_crc32 != flexibin_crc32) {
            console_printU5("[ERROR] schdr_check_flexibin(): check failed: component[0x%08x] expected crc32=0x%x, got 0x%x\n", component->hdr.identifier, component->hdr.reserved_crc32, flexibin_crc32, 0U, 0U);
            retval = ERR_INTF_FLEXIDAG_BIN_CRC_MISMATCHED;
        } else {
            supported_crc32 = 1U;
        }
    }

    if (supported_crc32 == 1U) {
        num_components = component->hdr.size / ((uint32_t)sizeof(flexibin_main_header_t));
        for (i = 1U; i < num_components; i++) {
            typecast(&pBuffer_u64,&pBuffer);
            pBuffer_u64 = pBuffer_u64 + ((uint64_t)sizeof(flexibin_main_header_t) * (uint64_t)i);
            typecast(&component,&pBuffer_u64);
            typecast(&pBuffer_u64,&pBuffer);
            pBuffer_u64 = pBuffer_u64 + (uint64_t)component->hdr.offset;
            typecast(&pBuffer_u8,&pBuffer_u64);
            flexibin_crc32 = check_crc32(pBuffer_u8, component->hdr.size);
            if (component->hdr.reserved_crc32 != flexibin_crc32) {
                console_printU5("[ERROR] schdr_check_flexibin(): check failed: component[0x%08x] expected crc32=0x%x, got 0x%x\n", component->hdr.identifier, component->hdr.reserved_crc32, flexibin_crc32, 0U, 0U);
                retval = ERR_INTF_FLEXIDAG_BIN_CRC_MISMATCHED;
            }
        }
    }

    return retval;
}
