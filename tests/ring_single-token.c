
#include <stdio.h>
#include <shmem.h>
#include <shmemx.h>

#define HANDLER_REQ   420
#define HANDLER_REPLY 240

int flag = 0;
shmemx_am_mutex lock_flag;

void sample_req_handler(void* temp_addr, size_t nbytes, int req_pe, shmemx_am_token_t token)
{
  
  shmemx_am_mutex_lock(&lock_flag);
  flag = 1;
  shmemx_am_mutex_unlock(&lock_flag);
}

long pSync[SHMEM_BARRIER_SYNC_SIZE];

int main(int argc, char **argv)
{
  const long int ITER_CNT = 100;
  int peer, i, pe_cnt, pe_id;
  long long int start_time, stop_time, res;
  double time;
  long int j=0; 

  shmem_init();
  pe_id = shmem_my_pe();
  for (i = 0; i < SHMEM_BARRIER_SYNC_SIZE; i += 1)
      pSync[i] = SHMEM_SYNC_VALUE;
  shmemx_am_attach(HANDLER_REQ, &sample_req_handler  );
  get_rtc_res_(&res);

  for(pe_cnt=2; pe_cnt<=shmem_n_pes(); pe_cnt*=2) {
      peer = (pe_id+1)%pe_cnt;
      flag = 0;
      shmem_barrier_all();

      if(pe_id<pe_cnt) {

         shmem_barrier(0,0,pe_cnt,pSync);
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
            printf("%20ld;%20.12f;%20.12f\n", 
                   pe_cnt, time, (double)time/pe_cnt);
         }
         fflush(stdout);
      }
  }
  shmem_finalize();
}

