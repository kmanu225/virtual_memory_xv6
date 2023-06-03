#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int fib(int* tableau, int n){

    if(n <=0)
      return 0;
    if (n == 1)
      return 1;
    tableau[n] = fib(tableau, n-1)+fib(tableau, n-2);
    return tableau[n];
}

int
main(int argc, char **argv)
{
  if(argc < 2){
    printf("Usage: %s N\n", argv[0]);
    exit(1);
  }
  int tableau[800];
  int n = atoi(argv[1]);
  int res = fib(tableau, n);
  printf("fib(%d)=%d\n", n, res);
  exit(0);
}
