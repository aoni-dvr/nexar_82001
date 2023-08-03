#include "nvme_internal.h"
#include "AmbaRTSL_TMR.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaPrint.h"

//#define WRAP_DBG

UINT  _txe_byte_pool_create(TX_BYTE_POOL *pool_ptr, CHAR *name_ptr, VOID *pool_start, UINT32 pool_size, UINT pool_control_block_size);
UINT  _txe_byte_allocate(TX_BYTE_POOL *pool_ptr, VOID **memory_ptr,
                                    UINT32 memory_size,  UINT32 wait_option);
UINT  _txe_byte_release(VOID *memory_ptr);

inline uint32_t nvme_mmio_read_4(void *mem)
{
    return *(volatile uint32_t *)mem;
}

inline uint64_t nvme_mmio_read_8(uint64_t *mem)
{
    uint32_t data_lo = *(volatile uint32_t *)mem;
    uint32_t data_hi = *((volatile uint32_t *)mem + 1);

    return ((uint64_t)data_hi << 32) | (uint64_t)data_lo;
}

inline void nvme_mmio_write_4(volatile uint32_t *mem, uint32_t val)
{
    *mem = val;
}

inline void nvme_mmio_write_8(volatile uint64_t *mem, uint64_t val)
{
    *(volatile uint32_t*)mem = val & 0xFFFFFFFF;
    *((volatile uint32_t*)mem + 1) = (uint32_t)(val >> 32);
}

uint32_t nvme_time_msec(void)
{
    AMBA_TMR_INFO_s TimerInfo;

    // get time stamp from sys tick
    // expire count increases every ms
    AmbaRTSL_TmrGetInfo(AMBA_TIMER_SYS_TICK, &TimerInfo);

    return TimerInfo.ExpireCount;
}

static TX_BYTE_POOL malloc_pool_ = {0};

// setup memory pool
uint32_t nvme_mem_pool_init(nvme_sys_config_s *config)
{
    int ret;
    ret = _txe_byte_pool_create(&malloc_pool_, "Nvme Pool", config->StackNonCacheMemory, config->StackNonCacheMemorySize, sizeof(TX_BYTE_POOL));

    AmbaPrint_PrintUInt5("Create non-cache pool for NVME Addr = 0x%x, Size = 0x%x, Ret = 0x%x",
            config->StackNonCacheMemory,
            config->StackNonCacheMemorySize,
            ret, 0, 0);

    return ret;
}

void *threadx_malloc(size_t size)
{
    void * ptr = NULL;

    if(size > 0) {
        // We simply wrap the threadX call into a standard form
        uint8_t r = _txe_byte_allocate(&malloc_pool_, &ptr, size, 100);
#ifdef WRAP_DBG
        AmbaPrint_PrintUInt5("threadx_malloc(): size = %d return = %d", size, r, 0, 0, 0);
#endif
        if(r != TX_SUCCESS) {
            ptr = NULL;
        } else {
            memset(ptr, 0, size);
        }
    }
    //else NULL if there was no size

    return ptr;
}

void *threadx_calloc(size_t n, size_t size)
{
    void * ptr = NULL;
    size_t to_allocated = n * size;

    if (to_allocated > 0) {
        //Allocate a n * size byte memory area from my_pool.
        uint8_t r = _txe_byte_allocate(&malloc_pool_, (VOID **) &ptr, to_allocated, 100);

        if (r != TX_SUCCESS) {
            ptr = NULL;
        } else {
            memset(ptr, 0, to_allocated);
        }
    }

    return ptr;
}

void threadx_free(void * ptr)
{
    if(ptr) {
        //We simply wrap the threadX call into a standard form
        uint8_t r = _txe_byte_release(ptr);
    }
}

uint64_t wrap_nvme_mem_vtophys(void *virt)
{
    // virt = phys;
    return (uint64_t)virt;
}

void *nvme_malloc_node(size_t size, size_t align, unsigned int node_id)
//void *wrap_nvme_malloc_node(size_t size, size_t align)
{
    uintptr_t mask = ~(uintptr_t)(align - 1);
    void *mem = malloc(size+align-1);
    void *ptr = (void *)(((uintptr_t)mem+align-1) & mask);
    (void)node_id;

    return ptr;
}

void *wrap_nvme_mem_alloc_node(size_t size, size_t align, uint64_t *paddr)
{
    uintptr_t mask = ~(uintptr_t)(align - 1);
    void *mem = malloc(size+align-1);
    void *ptr = (void *)(((uintptr_t)mem+align-1) & mask);

    // virt = phys;
    *paddr = (uint64_t)ptr;
#ifdef WRAP_DBG
    AmbaPrint_PrintUInt5("wrap_nvme_mem_alloc_node(): addr = 0x%x.%x", (uint32_t)(*paddr >> 32), (uint32_t)(*paddr & 0xFFFFFFFF), 0, 0, 0);
#endif
    return ptr;
}

