
#include <stdio.h>
#include <shmem.h>
#include <shmemx.h>

#define HANDLER_ID_1 420

volatile int flag = 0;
volatile int count = 0; 
int pe_id;

void sample_req_handler(void* temp_addr, size_t nbytes, int req_pe)
{
	flag+=*((int*)temp_addr);
	count++;
}


int main(int argc, char **argv)
{
  int source_addr[1], peer, i; 
  const int MSG_CNT = 3;

  shmem_init();
  pe_id = shmem_my_pe();
  peer = (pe_id+1)%shmem_n_pes();

  /* initializing data payload with one integer */
  source_addr[0]=0;
  shmem_barrier_all();

  /* Collective operation: Implicit barrier on return from attach */
  shmemx_am_attach(HANDLER_ID_1, &sample_req_handler);

  /* Launch MSG_CNT active messages */
  for(i=1; i<=MSG_CNT; i++) {
	source_addr[0] = i*(pe_id+1);
  	shmemx_am_launch(peer, HANDLER_ID_1, source_addr, sizeof(int));
  }

  shmemx_am_quiet();		
  shmem_barrier_all();
  printf("%d: Sent %d integer(s) to PE%d. Sum = %d\n", pe_id, MSG_CNT, peer, MSG_CNT*(MSG_CNT+1)/2*(pe_id+1));
  shmem_barrier_all();
  printf("%d: Received %d integer(s) from PE%d. Sum = %d\n", pe_id, count, peer, flag);

  shmem_finalize();
}

