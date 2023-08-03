#include "AmbaIntrinsics.h"

typedef uint64_t phys_addr_t;
typedef uint64_t __u64;
typedef uint32_t __u32;

#define PAGE_SIZE 4*1024
#define PAGE_SHIFT 12

#define EINVAL  0x01
#define ENOMEM  0x02
#define ENXIO   0x03

#define NVME_PCI_VID_INTEL 0xFFFF
#define NVME_VTOPHYS_ERROR 0xFFFF

uint32_t nvme_mmio_read_4(void *mem);
uint64_t nvme_mmio_read_8(uint64_t *mem);
void nvme_mmio_write_4(volatile uint32_t *mem, uint32_t val);
void nvme_mmio_write_8(volatile uint64_t *mem, uint64_t val);

uint32_t nvme_time_msec(void);

#define nvme_min(a,b) (((a)<(b))?(a):(b))

typedef struct {
    uint8_t           *StackCacheMemory;       // [Input] static memory for nvme stack usage (cacheable)
    uint32_t          StackCacheMemorySize;    // [Input] Size of static memory for nvme stack usage (cacheable)
    uint8_t           *StackNonCacheMemory;    // [Input] static memory for nvme stack usage (non-cacheable)
    uint32_t          StackNonCacheMemorySize; // [Input] Size of static memory for nvme stack usage (non-cacheable)
} nvme_sys_config_s;

uint32_t nvme_mem_pool_init(nvme_sys_config_s *config);
void *threadx_calloc(size_t n, size_t size);
void *threadx_malloc(size_t size);
void threadx_free(void * ptr);

uint64_t wrap_nvme_mem_vtophys(void *virt);
void *wrap_nvme_malloc_node(size_t size, size_t align);
void *wrap_nvme_mem_alloc_node(size_t size, size_t align, uint64_t *paddr);

#define nvme_err
#define nvme_notice
#define nvme_debug
#define nvme_warning
#define nvme_crit
#define nvme_panic
#define nvme_info

#define strerror

#define nvme_assert

//#define nvme_malloc_node(a,b,c)       wrap_nvme_malloc_node(a,b)
#define nvme_free

#define nvme_mem_vtophys              wrap_nvme_mem_vtophys
#define nvme_mem_alloc_node(a,b,c,d)  wrap_nvme_mem_alloc_node(a,PAGE_SIZE,d)

#define _SC_PAGESIZE 0xFFFF //keep compiler happy
#define sysconf


#define calloc  threadx_calloc
#define malloc  threadx_malloc
#define free    threadx_free

/*
 * Write memory barrier.
 * Guarantees that the STORE operations generated before the barrier
 * occur before the STORE operations generated after.
 * This function is architecture dependent.
 */
#define nvme_wmb	 AMBA_DMB
