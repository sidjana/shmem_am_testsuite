
#include <stdio.h>
#include <shmem.h>
#include <shmemx.h>

#define HANDLER_REQ   420
#define HANDLER_REPLY 240

volatile int flag = 0;
int pe_id;

void sample_req_handler(void* temp_addr, size_t nbytes, int req_pe, shmemx_am_token_t token)
{
  printf("PE%d: Inside request handler. Received data = %d from PE%d\n",
          pe_id, *((int*)temp_addr), req_pe);
  int new_buff = *((int*)temp_addr)+1;
  shmemx_am_reply(HANDLER_REPLY, &new_buff, sizeof(int), token );
}

void sample_reply_handler(void* temp_addr, size_t nbytes, int req_pe, shmemx_am_token_t token)
{
  printf("PE%d: Inside reply handler. Received data = %d from PE%d\n",
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
  shmemx_am_attach(HANDLER_REQ,   &sample_req_handler  );
  shmemx_am_attach(HANDLER_REPLY, &sample_reply_handler);
  shmemx_am_request(peer, HANDLER_REQ, source_addr, sizeof(int));
  shmemx_am_quiet();		
  shmem_barrier_all();

  shmem_finalize();
}

