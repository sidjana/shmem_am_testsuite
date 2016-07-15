
#include <stdio.h>
#include <stdlib.h>
#include <shmem.h>
#include "../support/rtc.h"

#define HANDLER_ID_REP 420
#define HANDLER_ID_REQ 421

long pSync[SHMEM_BARRIER_SYNC_SIZE];


int main(int argc, char **argv)
{
  const long int ITER_CNT = 100;
  const long int MAX_MSG_CNT = 1048576;
  int* source_addr;
  int peer;
  long int i, j, msg_cnt; 
  long long int start_time, stop_time, res;
  double time;

  shmem_init();

  int pe_id = shmem_my_pe();
  source_addr = (int*) shmem_malloc(sizeof(int));

  if(pe_id == 1) {
      if(shmem_n_pes()!=4)
      	fprintf(stderr,"Num PEs should be ==4");
      printf("#Message Cnt;Time(s);MR(msgs/sec)\n");
  }

  if (pe_id==1)
	  peer = 3;
  else if(pe_id==3)
	  peer = 1;
  get_rtc_res_(&res);

  for (i = 0; i < SHMEM_BARRIER_SYNC_SIZE; i += 1){
          pSync[i] = SHMEM_SYNC_VALUE;
  }

  shmem_barrier_all();
  if(pe_id == 1 || pe_id == 3) {

  	for(msg_cnt=1; msg_cnt<=MAX_MSG_CNT; msg_cnt*=2) {
  	    shmem_barrier(1,1,2,pSync);
  	    get_rtc_(&start_time);
  	    	 for(j=1;j<=ITER_CNT;j++) {
  	    	   for(i=1;i<=msg_cnt;i++)
		   	shmem_int_put(source_addr,source_addr,1,peer);
		   shmem_quiet();
  	    	 }
  	    shmem_barrier(1,1,2,pSync);
  	    get_rtc_(&stop_time);
  	    time = (stop_time - start_time)*1.0/(double)res/ITER_CNT;
  	    if(pe_id == 1) {
  	   	 printf("%20ld;%20.12f;%20.12f\n", 
  	                msg_cnt, time, (double)msg_cnt/time);
  	    }
  	    fflush(stdout);
  	}
  }

  shmem_barrier_all();
  shmem_finalize();

}

