
#include <stdio.h>
#include <shmem.h>
#include <shmemx.h>
#include "rtc.h"

#define HANDLER_ID_1 420

volatile int count = 0; 
int pe_id;

void sample_req_handler(void* temp_addr, size_t nbytes, int req_pe)
{
	count++;
}


int main(int argc, char **argv)
{
  const long int MSG_CNT = 1048576;
  const int BUFF_SIZE = 1024;
  int source_addr[1];
  long int i=0, j=0; 
  long long int start_time, stop_time, res;

  shmem_init();
  pe_id = shmem_my_pe();

  /* Collective operation: Implicit barrier on return from attach */
  shmemx_am_attach(HANDLER_ID_1, &sample_req_handler);

  /* Launch MSG_CNT active messages with payload size = BUFF_SIZE each*/
  if(pe_id == 0) { 
	get_rtc_res_(&res);
	printf("#Messages\tTime(s)\n");
  	for(i=128; i<=MSG_CNT; i*=2) {
  	      get_rtc_(&start_time);
  	      for(j=1;j<=i;j++) {
  			shmemx_am_launch(1, HANDLER_ID_1, source_addr, BUFF_SIZE);
  	      }
  	      shmemx_am_quiet();
  	      get_rtc_(&stop_time);
  	      printf("%ld\t%20.12f\n", i, (stop_time - start_time)*1.0/(double)res);
	      fflush(stdout);
  	}
  }

  shmem_finalize();
}

