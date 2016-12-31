#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"


int
main(int argc, char *argv[])
{
  int fd;
  struct stat st;

  if(argc < 2){
    printf(2, "Usage: filestat <filename>\n");
    exit();
 } 

  fd = open(argv[1], O_RDWR);
  if(fd < 0){
    printf(2, "%s failed to open\n", argv[1]);
    exit();
  }

   if(fstat(fd, &st) < 0){
    printf(2, "Failed to acquire stat information\n");
    exit();
  }

  //print st information
  printf(1, "Filename: %s\n", argv[1]);
  switch(st.type){
  case 1:
  printf(1, "    Type of file: T_DIR\n");
  break;
  case 2:
  printf(1, "    Type of file: T_FILE\n");
  break;
  case 3:
  printf(1, "    Type of file: T_DEV\n");
  break;
  case 4:
  printf(1, "    Type of file: T_CHECKED\n");
  break;
  default:
  printf(2, "Error, unknown file type\n");
  exit();
  }
  printf(1, "    Device number: %d\n", st.dev);
  printf(1, "    Inode number on device: %d\n", st.ino);
  printf(1, "    Number of links to file: %d\n", st.nlink);
  printf(1, "    Size of file in bytes: %d\n", st.size);
  if(st.type==4)
  printf(1, "    Checksum of file: %d\n", st.checksum);
  else 
  printf(1, "    Checksum of file: unknown\n");
   
  exit();
}
