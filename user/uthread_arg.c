#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/uthread.h"

// stacks for user-level threads
#define STACK_SIZE 4096
uint8 foo_stack[STACK_SIZE] __attribute__((aligned(16)));

void foo(uint64 arg) {
    printf("Hello! %s.\n", (char *)arg);
    uthread_exit();
}

int main() {
    uthread_add2(foo, (uint64)"World", foo_stack, STACK_SIZE);
    uthread_start();
    exit(0);
}
