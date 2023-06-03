#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/riscv.h"

int main(int argc, char** argv){
  uint64 BIG= 25 * PGSIZE;
  char *a, *lastaddr;
  uint64 amt;

  printf("Coucou\n"); // Initialise le buffer dans printf
  // Effectivement, lui aussi, fait un sbrk...

  // Break courant
  a = sbrk(0);
  printf("a = %p\n", a);
  a = sbrk(0);
  printf("a = %p\n", a);
  a = sbrk(0);
  printf("a = %p\n", a);

  BIG = (uint64)a + PGSIZE;
  amt = BIG - (uint64)a;
  printf("BIG= %p\n", BIG);
  printf("amt= %p\n", amt);
  sbrk(amt);

  //
  lastaddr = (char*) (BIG-1);
  printf("lastaddr = %p\n", lastaddr);
  *lastaddr = 99; // 1er page fault
  *a = 11;
  sbrk(-PGSIZE);
  sbrk(PGSIZE);
  printf("%d\n", *a); // 2eme page fault
  printf("%d\n", *lastaddr);
  if(*lastaddr == 99){
    printf("sbrk de-allocation didn't really deallocate\n");
    exit(1);
  }
  exit(0);

  /* char* a = sbrk(PGSIZE); */
  /* *a = 2; */
  /* sbrk(-PGSIZE); */
  /* sbrk(PGSIZE); */
  /* printf("%d\n", *a); */
  /* exit (0); */
}
