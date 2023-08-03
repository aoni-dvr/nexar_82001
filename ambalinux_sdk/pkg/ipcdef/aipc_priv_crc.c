#include <stdlib.h>
#include <stdio.h>

#include "aipc_priv_crc.h"
#include "priv_crc.h"
unsigned int getPrivCrc(int prog){
	int i;
	for(i=0; i<priv_rpc_num; i++){
		if(priv_rpc_prog_id[i]==prog){	//look for the index of crc_result
			return priv_crc_result[i];
		}
	}
	
	/* can not find the program id */
	return 0;
	
}
