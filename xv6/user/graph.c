#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"


int
main(int argc, char *argv[])
{
   struct pstat st;
   //workload 
   int i;
   int x=0;
   int j;

   for (i = 1; i<atoi(argv[1]);i++) {
   x+=i;		 
   getpinfo(&st);
   for(i = 0; i < NPROC; i++) {
      if (st.inuse[i]) {
	  printf(1, "pid: %d priority: %d\n ", st.pid[i], st.priority[i]);
	  for (j = 0; j < 4; j++)
	      printf(1, "\t level %d ticks used %d\n", j, st.ticks[i][j]);  
      }
   }

}

   exit();
   return 0;
}
