#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/riscv.h"


int main(int argc, char **argv)
{
	sbrk(-(uint64)sbrk(0));
	printf("This is the end.\n");
	exit(0);
}

