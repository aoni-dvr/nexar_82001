/**
 *  This is the test module for libambamem.
 *  input Addr_rtos_target and map to Addr_lnx_target
 *      Addr_lnx_target = Addr_rtos_target + offset(Addr_lnx_base, Addr_rtos_base)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libambamem.h"


static void
usage(char *pProg_name)
{
    fprintf(stderr,
        "\nusage: %s [target physical address]\n"
        "\te.g. %s 3d303000\n"
        "\n",
        pProg_name, pProg_name);
    return;
}

int main(int argc, char** argv)
{
	int rval;
	long long offset_h;
	long long offset_d;
	void *src_addr = NULL;
	void *mmap_addr = NULL;
    unsigned long long t_addr;

	if( argc < 2 )
	{
	    usage(argv[0]);
	    return 0;
	}

	t_addr = strtoull (argv[1], NULL, 16);
	if (t_addr == 0) {
		printf("Invalid input address or Please use addr larger than 0!\n");
		return -1;
	}
	src_addr = (void *)t_addr;

//init
	rval = AmbaMem_init();
	if(rval != 0){
		printf("AmbaMem_init() fail!\n\r");
		return rval;
	}
	printf("AmbaMem_init() ok!\n\r");

//Do mmap HEAP
	rval = AmbaMem_mmap(AMBAMEM_TYPE_HEAP, &offset_h);
	if(rval != 0){
		printf("AmbaMem_mmap(AMBAMEM_TYPE_HEAP) fail!\n\r");
		return rval;
	}
	printf("AmbaMem_mmap(AMBAMEM_TYPE_HEAP) ok, offset(Linux_base_addr, RTOS_base_addr)= x%llx!\n\r", offset_h);

//Do mmap DSP
	rval = AmbaMem_mmap(AMBAMEM_TYPE_DSP, &offset_d);
	if(rval != 0){
		printf("AmbaMem_mmap(AMBAMEM_TYPE_DSP) fail!\n\r");
		return rval;
	}
	printf("AmbaMem_mmap(AMBAMEM_TYPE_DSP) ok, offset(Linux_base_addr, RTOS_base_addr)= x%llx!\n\r", offset_d);

//Do some operation on HEAP
	mmap_addr = AmbaMem_convert(AMBAMEM_TYPE_HEAP, src_addr);
	if(mmap_addr!=NULL){
		int i;
		unsigned int *t_ptr;
		unsigned int t_val;

		printf("src: %p, mmap: %p\n",src_addr, mmap_addr);
		t_ptr = (unsigned int *) mmap_addr;
		for (i=0; i<11; i++) {
			t_val = 0xfeedda00 + i;
			*t_ptr = t_val;
			printf("write 0x%08x to %p\n",t_val, t_ptr);
			sleep(1);
		}
	}

// Do munmap HEAP
	rval = AmbaMem_munmap(AMBAMEM_TYPE_HEAP);
	if(rval != 0){
		printf("AmbaMem_munmap(AMBAMEM_TYPE_HEAP) fail!\n\r");
		return rval;
	}
	printf("AmbaMem_munmap(AMBAMEM_TYPE_HEAP) ok!\n\r");

// Do munmap DSP
	rval = AmbaMem_munmap(AMBAMEM_TYPE_DSP);
	if(rval != 0){
		printf("AmbaMem_munmap(AMBAMEM_TYPE_DSP) fail!\n\r");
		return rval;
	}
	printf("AmbaMem_munmap(AMBAMEM_TYPE_DSP) ok!\n\r");

	rval = AmbaMem_release();
	if(rval != 0){
		printf("AmbaMem_release() fail!\n\r");
		return rval;
	}

	return rval;
}