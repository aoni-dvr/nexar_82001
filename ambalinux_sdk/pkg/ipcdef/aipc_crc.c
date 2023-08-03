#include <stdlib.h>
#include <stdio.h>

#include "aipc_crc.h"
#include "crc.h"
unsigned int getCrc(int prog){
	int i;
	for(i=0; i<rpc_num; i++){
		if(rpc_prog_id[i]==prog){	//look for the index of crc_result
			return crc_result[i];
		}
	}
	
	/* can not find the program id */
	return 0;
	
}
