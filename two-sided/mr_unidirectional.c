
#include <stdio.h>
#include <stdlib.h>
#include <shmem.h>
#include <shmemx.h>
#include "../support/rtc.h"

#define HANDLER_ID_REP 420
#define HANDLER_ID_REQ 421

volatile int count = 0; 

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
  const long int ITER_CNT = 1000;
  const long int MAX_MSG_CNT = 1048576;
  int* source_addr;
  int peer;
  long int i, j, msg_cnt; 
  long long int start_time, stop_time, res;
  double time;

  shmem_init();

  int pe_id = shmem_my_pe();
  source_addr = (int*) malloc(MAX_MSG_CNT);

  if(pe_id == 0) {
      if(shmem_n_pes()<2)
      	fprintf(stderr,"Num PEs should be >=2");
      printf("Message Cnt\tTime(s)\tMR(msgs/sec)\n");
  }
  peer = (pe_id)?0:1;
  get_rtc_res_(&res);

  /* Collective operation: Implicit barrier on return from attach */
  shmemx_am_attach(HANDLER_ID_REQ, &sample_req_handler);
  shmemx_am_attach(HANDLER_ID_REP, &sample_rep_handler);

  for(msg_cnt=1; msg_cnt<=MAX_MSG_CNT; msg_cnt*=2) {
      get_rtc_(&start_time);
      if(pe_id == 0) {
      	 for(j=1;j<=ITER_CNT;j++) {
      	     for(i=1;i<=msg_cnt;i++)
      	        shmemx_am_request(peer, HANDLER_ID_REQ, NULL, 0);
             shmemx_am_quiet();
      	 }
      }
      shmem_barrier_all();
      get_rtc_(&stop_time);
      time = (stop_time - start_time)*1.0/(double)res/ITER_CNT;
      if(pe_id == 0) {
     	 printf("%20ld\t%20.12f\t%20.12f\n", 
                  msg_cnt, time, (double)msg_cnt/time);
      }
      fflush(stdout);
  }

  shmem_barrier_all();
  shmem_finalize();

}

