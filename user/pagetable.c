#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(){
    int pid = getpid();
    printf("Pagetable for PID = %d\n", pid);
    int ret = dump_pagetable(pid);
    // printf("pagetable returned %d\n", ret);
    exit(ret);
}

