// Do not modify this file. It will be replaced by the grading scripts
// when checking your project.

#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  char *p = (char *)(640*1024 - 4096);
  *p = 'a';
  printf(1, "value of pointer p %x\n", p);
  printf(1, "value pointed by p %x\n", *p);
  exit();
}
