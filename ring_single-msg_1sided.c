
#include <stdio.h>
#include <shmem.h>
#include <shmemx.h>

#define HANDLER_ID_1 420

volatile int flag = 0;
int pe_id;

void sample_req_handler(void* temp_addr, size_t nbytes, int req_pe)
{
  printf("PE%d: Inside request handler. Received data = %d from PE%d\n",
          pe_id, *((int*)temp_addr), req_pe);
}


int main(int argc, char **argv)
{
  int peer;
  int source_addr[1]; 

  shmem_init();
  pe_id = shmem_my_pe();
  peer = (pe_id+1)%shmem_n_pes();

  /* initializing data payload with one integer */
  source_addr[0]=6*peer;

  /* Collective operation: Implicit barrier on return from attach */
  shmemx_am_attach(HANDLER_ID_1, &sample_req_handler);
  shmemx_am_launch(peer, HANDLER_ID_1, source_addr, sizeof(int));
  shmemx_am_quiet();		

  shmem_finalize();
}

