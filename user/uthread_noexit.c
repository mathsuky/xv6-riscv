#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/uthread.h"

// stacks for user-level threads
#define STACK_SIZE 4096
uint8 foo_stack[STACK_SIZE] __attribute__((aligned(16)));
uint8 bar_stack[STACK_SIZE] __attribute__((aligned(16)));
uint8 baz_stack[STACK_SIZE] __attribute__((aligned(16)));

void foo_no_exit(void) {
    int c = 0;
    for (int i = 0; i < 3; i++) {
        printf("foo_no_exit (tid=%d): %d\n", uthread_gettid(), c);
        c += 1;
        uthread_yield();
    }
    printf("foo_no_exit (tid=%d): finished without calling uthread_exit\n", uthread_gettid());
    // Intentionally omit uthread_exit() to test automatic exit
}

void bar_no_exit(void) {
    int c = 0;
    for (int i = 0; i < 3; i++) {
        printf("bar_no_exit (tid=%d): %d\n", uthread_gettid(), c);
        uthread_yield();
        c += 2;
    }
    printf("bar_no_exit (tid=%d): finished without calling uthread_exit\n", uthread_gettid());
    // Intentionally omit uthread_exit() to test automatic exit
}

void baz_no_exit_sub(int *cp) {
  printf("baz_no_exit (tid=%d): %d\n", uthread_gettid(), *cp);
  uthread_yield();
  *cp += 3;
}

void baz_no_exit(void) {
    int c = 0;
    for (int i = 0; i < 3; i++) {
        baz_no_exit_sub(&c);
        baz_no_exit_sub(&c);
    }
    printf("baz_no_exit (tid=%d): finished without calling uthread_exit\n", uthread_gettid());
    // Intentionally omit uthread_exit() to test automatic exit
}

int main(void) {
  printf("Testing automatic uthread_exit functionality\n");
  printf("All thread functions will omit uthread_exit() calls\n");
  
  uthread_add(foo_no_exit, foo_stack, STACK_SIZE);
  uthread_add(bar_no_exit, bar_stack, STACK_SIZE);
  uthread_add(baz_no_exit, baz_stack, STACK_SIZE);
  
  uthread_start();
  
  printf("All threads completed successfully without explicit uthread_exit calls\n");
  exit(0);
}