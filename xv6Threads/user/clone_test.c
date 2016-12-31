#include "types.h"
#include "stat.h"
#include "user.h"


static void thread_start(void* arg) {
  int i=0;
  int pid = getpid();

  for(i = 0; i< 5; i++){
    printf(1, "Thread pid %d is running %dth iteration\n",pid,i );
  }

  while(1){
  sleep(2);
  };
}

int main() {
  int tid;
  int pid = getpid();
  int j;

  printf(1, "main pid %d is running \n",pid );

  for (j=0; j<5; j++) {
    tid = thread_create(&thread_start, NULL);
    printf(1,"Created thread with TID %d\n", tid);
  }
  printf(1, "Done with thread creation\n");

  while(1) {
   sleep(2);
  };
  return 0;
}
