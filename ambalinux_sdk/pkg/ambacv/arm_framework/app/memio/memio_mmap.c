#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>
#include "cvtask_api.h"
#include "ppm.h"
#include "memio_mmap.h"


struct ambacv_mem_region {
	uint64_t    base;
	uint64_t    size;
};

struct ambacv_mem {
	struct ambacv_mem_region all;
	struct ambacv_mem_region sod;
	struct ambacv_mem_region vp;
	struct ambacv_mem_region tbar;
	struct ambacv_mem_region arm;
	struct ambacv_mem_region hotlink;
	struct ambacv_mem_region cavalry_block;
};

struct dram_region {
	char*     va_base;
	uint32_t  pa_base;
	int       mm_size;
};


static struct dram_region       region;
static int        mem_fd;


static void map_region(struct dram_region *r)
{
	char *ptr;

	if (r->mm_size == 0)
		return;

	ptr = (char*)mmap(NULL, r->mm_size, PROT_READ | PROT_WRITE,
			MAP_SHARED, mem_fd, r->pa_base);
	assert(ptr != MAP_FAILED);

	r->va_base = ptr;
}

static void unmap_region(struct dram_region *r)
{
	if (r->mm_size == 0)
		return;

	munmap(r->va_base, r->mm_size);
	close(mem_fd);

	r->mm_size = 0;
}

int memio_mmap_init(uint32_t size, void** vrit_addr)
{
    int ret;
    struct PPM_MEM_INFO_s mem_info;

    mem_fd = open("/dev/ppm", O_RDWR);
    if (mem_fd < 0) {
        printf("Cannot open device file /dev/ppm !!!\n\n");
        exit(-1);
    }

    ret = ioctl(mem_fd, PPM_GET_MEMIO_INFO, &(mem_info));
    if(ret != 0){
        printf("Cannot retrieve MEMIO info!(%d)\n\r",ret);
    } else {
        region.pa_base = (uint32_t)mem_info.phys;
        printf("region info: virt=0x%lx, phys=0x%lx, size=0x%08x\n", region.va_base, region.pa_base, region.mm_size);
    }

    region.mm_size = size;
    map_region(&region);

    *vrit_addr = region.va_base;
	return 0;
}

void* memio_mmap_p2v(uint32_t pa)
{
	if (pa == 0)
		return NULL;

	if (pa >= region.pa_base ) {
		int offset = pa - region.pa_base;
		if (offset < region.mm_size) {
			return region.va_base + offset;
		}
	}

	printf("Error: memio pa address 0x%X out of range !\n", pa);
	//log_backtrace();
	assert(0);
	return NULL;
}


uint32_t memio_mmap_v2p(void *_va)
{
	char *va = (char*)_va;

	if (va == NULL)
		return 0;

	if (va >= region.va_base) {
		uint32_t offset = va - region.va_base;
		if (offset < region.mm_size) {
			return region.pa_base + offset;
		}
	}

	printf("Error: memio va address %p out of range !\n", va);
	//log_backtrace();
	assert(0);
	return 0;
}

uint32_t memio_mmap_v2off(void *_va)
{
	char *va = (char*)_va;

	if (va!=NULL && va >= region.va_base) {
		uint32_t offset = va - region.va_base;
		if (offset < region.mm_size)
			return offset;
	}

	printf("Error: memio va address %p out of range !\n", va);
	//log_backtrace();
	assert(0);
	return 0;
}

uint32_t memio_get_cvshm_info(unsigned int *base, unsigned int *size)
{
    #define AMBACV_GET_MEM_LAYOUT  _IOR ('v', 0x6, struct ambacv_mem *)
    struct ambacv_mem mem;
    uint32_t start, end;
    int ret;
    int ambacv_fd;

    ambacv_fd = open("/dev/ambacv", O_SYNC | O_RDWR);
    if (ambacv_fd < 0) {
        printf("Can't open /dev/ambacv !\n");
        return -1;
    }

    ret = ioctl(ambacv_fd, AMBACV_GET_MEM_LAYOUT, &mem);
    if (ret != 0) {
        printf("Can't get memory layout from kernel !\n");
        close(ambacv_fd);
        return -1;
    }

    if (!mem.all.size || !mem.all.base) {
        close(ambacv_fd);
        return -1;
    }

    // align memory region to 4K page
    start = mem.all.base & ~0xFFF;
    end = (mem.all.base + mem.all.size + 0xFFF) & ~0xFFF;

    if (base != NULL) {
        *base = start;
    }

    if (size != NULL) {
        *size = end - start;
    }

    printf("[memio_get_cvshm_info] ambacv mem base=0x%08x, size=0x%08x\n",mem.all.base,mem.all.size);
    close(ambacv_fd);
    return 0;
}

