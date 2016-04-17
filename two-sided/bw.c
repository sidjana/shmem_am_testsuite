
#include <stdio.h>
#include <stdlib.h>
#include <shmem.h>
#include <shmemx.h>
#include "rtc.h"

#define HANDLER_ID_REP 420
#define HANDLER_ID_REQ 421

volatile int count = 0; 
int pe_id;

void sample_req_handler(void *buf, size_t nbytes, int req_pe, shmemx_am_token_t token)
{
	shmemx_am_reply(HANDLER_ID_REP, buf, nbytes, token);
}


void sample_rep_handler(void *buf, size_t nbytes, int req_pe, shmemx_am_token_t token)
{
	count++;
}


int main(int argc, char **argv)
{
  const long int MSG_CNT = 1040678;
  const int BUFF_SIZE = 1;
  int* source_addr;
  int peer;
  long int i=0, j=0; 
  long long int start_time, stop_time, res;
  double time;

  shmem_init();
  pe_id = shmem_my_pe();
  source_addr = (int*) malloc(BUFF_SIZE);

  /* Collective operation: Implicit barrier on return from attach */
  shmemx_am_attach(HANDLER_ID_REQ, &sample_req_handler);
  shmemx_am_attach(HANDLER_ID_REP, &sample_rep_handler);

  /* Launch MSG_CNT active messages with payload size = BUFF_SIZE each*/
  if(pe_id == 1) { 
	peer = 2;
	if(shmem_n_pes()<3)
		fprintf(stderr,"Num PEs should be >=3");
	get_rtc_res_(&res);
	printf("Count\t#Messages\tTime(s)\t\tMessageRate(#msg/s)\n");
  	for(i=1; i<=MSG_CNT; i*=2) {
  	      get_rtc_(&start_time);
  	      for(j=1;j<=i;j++) {
  			shmemx_am_request(peer, HANDLER_ID_REQ, source_addr, BUFF_SIZE);
  	      }
  	      shmemx_am_quiet();
  	      get_rtc_(&stop_time);
	      time = (stop_time - start_time)*1.0/(double)res;
  	      printf("%5d\t%20ld\t%20.12f\t%20ld\n", count,
		      i, time, (long long int)((double)i/time));
	      count=0;
	      fflush(stdout);
  	}
  }

  shmem_barrier_all();
  shmem_finalize();
}

