
#include <stdio.h>
#include <shmem.h>
#include <shmemx.h>

#define HANDLER_REQ   420
#define HANDLER_REPLY 240

volatile int flag = 0;

void sample_req_handler(void* temp_addr, size_t nbytes, int req_pe, shmemx_am_token_t token)
{
  flag = 1;
}

long pSync[SHMEM_BARRIER_SYNC_SIZE];

int main(int argc, char **argv)
{
  const long int ITER_CNT = 1000;
  const long int MAX_PE_CNT = 24;
  int peer, i, pe_cnt, pe_id = shmem_my_pe();
  long long int start_time, stop_time, res;
  double time;
  long int j=0; 

  shmem_init();
  for (i = 0; i < SHMEM_BARRIER_SYNC_SIZE; i += 1)
      pSync[i] = SHMEM_SYNC_VALUE;
  shmemx_am_attach(HANDLER_REQ, &sample_req_handler  );

  for(pe_cnt=3; pe_cnt<=MAX_PE_CNT; pe_cnt+=3) {
      peer = (pe_id+1)%pe_cnt;
      flag = 0;
      shmem_barrier_all();

      if(pe_id<pe_cnt) {
         get_rtc_(&start_time);

         for(j=1;j<=ITER_CNT;j++) {

             if(pe_id==0)
		flag = 1;
             shmem_int_wait_until(&flag,SHMEM_CMP_EQ,1);
             flag = 0;
             shmemx_am_request(peer, HANDLER_REQ, NULL, 0);
             shmem_barrier(0,0,pe_cnt,pSync);
         }
         get_rtc_(&stop_time);
         time = (stop_time - start_time)*1.0/(double)res/ITER_CNT;
         if(pe_id == 0) {
            printf("%20ld\t%20.12f\t%20.12f\n", 
                   pe_cnt, time, (double)pe_cnt/time);
         }
         fflush(stdout);
      }
  }
  shmem_finalize();
}

