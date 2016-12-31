#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "elf.h"

int
exec(char *path, char **argv)
{
  char *s, *last;
  int i, off;
<<<<<<< HEAD
  uint argc, sz, sp, ustack[3+MAXARG+1], user_stack;
=======
  uint argc, sz, sp, ustack[3+MAXARG+1];
>>>>>>> 74767cd65802c5cd9a70fadb16166fd86e5939f7
  struct elfhdr elf;
  struct inode *ip;
  struct proghdr ph;
  pde_t *pgdir, *oldpgdir;

<<<<<<< HEAD
//  cprintf("NEHA EXEC : starting exec.c\n");
=======
>>>>>>> 74767cd65802c5cd9a70fadb16166fd86e5939f7
  if((ip = namei(path)) == 0)
    return -1;
  ilock(ip);
  pgdir = 0;

  // Check ELF header
  if(readi(ip, (char*)&elf, 0, sizeof(elf)) < sizeof(elf))
    goto bad;
  if(elf.magic != ELF_MAGIC)
    goto bad;

  if((pgdir = setupkvm()) == 0)
    goto bad;

  // Load program into memory.
  sz = 0;
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
      goto bad;
    if(ph.type != ELF_PROG_LOAD)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;
<<<<<<< HEAD
    if((sz = allocuvm(pgdir, PGSIZE + sz, PGSIZE + ph.va + ph.memsz)) == 0)
=======
    if((sz = allocuvm(pgdir, sz, ph.va + ph.memsz)) == 0)
>>>>>>> 74767cd65802c5cd9a70fadb16166fd86e5939f7
      goto bad;
    if(loaduvm(pgdir, (char*)ph.va, ip, ph.offset, ph.filesz) < 0)
      goto bad;
  }
  iunlockput(ip);
  ip = 0;

<<<<<<< HEAD
//Round up till page boundary ADDED
  sz = PGROUNDUP(sz);


  // Allocate a one-page stack at the next page boundary
  user_stack = 0;
  //if((sz = allocuvm(pgdir, sz, sz + PGSIZE)) == 0)
  if((user_stack = allocuvm(pgdir, USERTOP-PGSIZE,  USERTOP)) == 0)
    goto bad;

// ADDED for experiment
//  uint newstk = allocuvm(pgdir, USERTOP-PGSIZE, USERTOP);
//  if(newstk==0)
//  panic("don't know what to do \n");

  // Push argument strings, prepare rest of stack in ustack.
  //sp = sz;
  sp = user_stack;   //ADDED


=======
  // Allocate a one-page stack at the next page boundary
  sz = PGROUNDUP(sz);
  if((sz = allocuvm(pgdir, sz, sz + PGSIZE)) == 0)
    goto bad;

  // Push argument strings, prepare rest of stack in ustack.
  sp = sz;
>>>>>>> 74767cd65802c5cd9a70fadb16166fd86e5939f7
  for(argc = 0; argv[argc]; argc++) {
    if(argc >= MAXARG)
      goto bad;
    sp -= strlen(argv[argc]) + 1;
    sp &= ~3;
    if(copyout(pgdir, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      goto bad;
    ustack[3+argc] = sp;
  }
<<<<<<< HEAD

=======
>>>>>>> 74767cd65802c5cd9a70fadb16166fd86e5939f7
  ustack[3+argc] = 0;

  ustack[0] = 0xffffffff;  // fake return PC
  ustack[1] = argc;
  ustack[2] = sp - (argc+1)*4;  // argv pointer

  sp -= (3+argc+1) * 4;
  if(copyout(pgdir, sp, ustack, (3+argc+1)*4) < 0)
    goto bad;

<<<<<<< HEAD
  user_stack = (uint)PGROUNDDOWN(sp); //ADDED TELLS entry point for stack
  
//  cprintf("NEHA EXEC size sz %x\n", sz);
//  cprintf("NEHA EXEC stack pointer sp %x\n", sp);
//  cprintf("NEHA EXEC user_stack var value %x\n", user_stack);

=======
>>>>>>> 74767cd65802c5cd9a70fadb16166fd86e5939f7
  // Save program name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(proc->name, last, sizeof(proc->name));

  // Commit to the user image.
  oldpgdir = proc->pgdir;
  proc->pgdir = pgdir;
  proc->sz = sz;
<<<<<<< HEAD
  proc->user_stack = user_stack; //ADDED
=======
>>>>>>> 74767cd65802c5cd9a70fadb16166fd86e5939f7
  proc->tf->eip = elf.entry;  // main
  proc->tf->esp = sp;
  switchuvm(proc);
  freevm(oldpgdir);

  return 0;

 bad:
  if(pgdir)
    freevm(pgdir);
  if(ip)
    iunlockput(ip);
  return -1;
}
