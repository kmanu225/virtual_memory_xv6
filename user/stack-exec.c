#include "kernel/types.h"
#include "kernel/memlayout.h"
#include "user/user.h"

/*
  # test.S
  # riscv64-unknown-elf-as -march=rv64gc -fpic test.S
  # riscv64-unknown-elf-objdump -d /tmp/a.out

  # write(1, s, sizeof(s))

  # fd = 1
  li a0, 1
  # syscall write -> a7 = 16
  li a7, 16

  # looking for the address of the string to print,
  j .mystring
  .back:

  # computing the length of the string, look for 0 byte
  mv a2, a1
  .loop2:
  lbu t0, 0(a2)
  addi a2, a2, 1
  beq t0, zero, .endloop2
  j .loop2
  .endloop2:
  sub a2, a2, a1

  # Syscall !
  ecall
  ret
  .mystring:
  jal a1, .back
  .data
  .asciz "Hello"
*/


char code[] = {
    // li a0, 1
    0x05, 0x45,
    // li a7, 16
    0xc1, 0x48,
    // j .mystring
    0x21, 0xa8,
    // mv a2, a1
    0x2e, 0x86,
    // .loop2:
    // lbu t0, 0(a2)
    0x83, 0x42, 0x06, 0x00,
    // addi a2, a2, 1
    0x05, 0x06,
    // beqz t0, .endloop2
    0x63, 0x83, 0x02, 0x00,
    // j .loop2
    0xdd, 0xbf,
    // .endloop2
    // sub a2, a2, a1
    0x0d, 0x8e,
    // ecall
    0x73, 0x00, 0x00, 0x00,
    // ret
    0x82, 0x80,
    // jal     a1,6 <.back>
    0xef, 0xf5, 0xbf, 0xfe,
    'H', 'e', 'l', 'l', 'o', '!', 0x0a, 0};

void do_fork(void (*fn)(void))
{
  int pid = fork();
  if (pid < 0)
  {
    printf("fork failed\n");
    exit(1);
  }
  else if (pid == 0)
  {
    fn();
  }
  else
  {
    wait(0);
  }
}

void test_code1()
{
  ((void (*)(void))(code))();
  printf("code1: cette ligne devrait-elle s'afficher ?\n");
  exit(0);
}

void test_code2()
{
  char code2[100];
  for (int i = 0; i < sizeof(code); i++)
  {
    code2[i] = code[i];
  }

  ((void (*)(void))(code2))();
  printf("code2: cette ligne devrait-elle s'afficher ?\n");
  exit(0);
}

void test_code3()
{
  char *code3;
  code3 =  sbrk(sizeof(code));
  for (int i = 0; i < sizeof(code); i++)
  {
    code3[i] = code[i];
  }

  ((void (*)(void))(code3))();
  printf("code3: cette ligne devrait-elle s'afficher ?\n");
  exit(0);
}

int main(int argc, char *argv[])
{
  do_fork(test_code1);
  do_fork(test_code2);
  do_fork(test_code3);
  exit(0);
}
