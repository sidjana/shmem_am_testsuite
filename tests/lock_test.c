
#include <stdio.h>
#include <stdlib.h>
#include <shmem.h>
#include <shmemx.h>

#define HANDLER_ID_REP 420
#define HANDLER_ID_REQ 421

volatile long int shared_var;
shmemx_am_mutex shared_lock;


void sample_req_handler(void *buf, size_t nbytes, int req_pe, shmemx_am_token_t token)
{
	shmemx_am_mutex_lock(&shared_lock);
	shared_var = shared_var+2;
	shmemx_am_mutex_unlock(&shared_lock);
}


void client_func(volatile long int* var)
{
	shmemx_am_mutex_lock(&shared_lock);
	*var = *var+2;
	shmemx_am_mutex_unlock(&shared_lock);
}


int main(int argc, char **argv)
{
  const long int MSG_CNT = 1040678;
  long int i=0, j=0; 
  int pe_id;

  shmem_init();
  pe_id = shmem_my_pe();

  shared_var = 0;
  shmemx_am_attach(HANDLER_ID_REQ, &sample_req_handler);
  shmemx_am_mutex_init(&shared_lock);
  shmem_barrier_all();

  for (i=0;i<MSG_CNT;i++) {
	  if(pe_id == 1)
  		shmemx_am_request(0, HANDLER_ID_REQ, NULL, 0);
          else {
	  	client_func(&shared_var);
	  }
	  shmemx_am_poll();
  }

  shmem_barrier_all();
  if(pe_id==0) 
  	printf("Calculated Result: %ld\t Expected Result: %ld\n",shared_var,MSG_CNT*4);
  shmemx_am_mutex_destroy(&shared_lock);

  shmem_finalize();
  return 0;
}

