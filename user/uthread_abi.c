#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/uthread.h"

// stacks for user-level threads
#define STACK_SIZE 4096
uint8 stack1[STACK_SIZE] __attribute__((aligned(16)));
uint8 stack2[STACK_SIZE] __attribute__((aligned(16)));

// グローバル変数で引数を比較
uint64 expected_arg = 0;
int test_passed = 0;

// テスト1: コンテキストスイッチ後のa0レジスタテスト
void thread1(uint64 arg) {
    printf("Thread1 start - arg=%d\n", (int)arg);
    expected_arg = arg;
    
    // 他のスレッドに譲る（重要：ここでコンテキストスイッチが発生）
    uthread_yield();
    
    // コンテキストスイッチ後、a0レジスタの値をチェック
    uint64 a0_after_yield;
    __asm__ volatile("mv %0, a0" : "=r"(a0_after_yield));
    
    printf("Thread1 after yield - arg=%d, a0=%d\n", (int)arg, (int)a0_after_yield);
    
    if (arg == a0_after_yield) {
        printf("SUCCESS: a0 register preserved across yield!\n");
        test_passed = 1;
    } else {
        printf("FAILURE: a0 register corrupted after yield!\n");
        printf("  Expected a0=%d, got a0=%d\n", (int)arg, (int)a0_after_yield);
        test_passed = 0;
    }
    
    uthread_exit();
}

// テスト2: 異なる引数で干渉をチェック
void thread2(uint64 arg) {
    printf("Thread2 start - arg=%d\n", (int)arg);
    
    // a0レジスタを意図的に変更
    __asm__ volatile("li a0, 99999");
    
    printf("Thread2 modified a0 to 99999\n");
    uthread_yield();  // thread1に戻る
    
    uthread_exit();
}

// テスト3: 複数回のyieldテスト
void thread3(uint64 arg) {
    printf("Thread3 start - arg=%d\n", (int)arg);
    
    for (int i = 0; i < 3; i++) {
        uint64 a0_value;
        __asm__ volatile("mv %0, a0" : "=r"(a0_value));
        printf("Thread3 iteration %d - arg=%d, a0=%d\n", i, (int)arg, (int)a0_value);
        
        if (arg != a0_value) {
            printf("ERROR: a0 corrupted in iteration %d!\n", i);
        }
        
        uthread_yield();
    }
    
    uthread_exit();
}

int main() {
    printf("=== Strict RISC-V ABI Test ===\n");
    printf("Testing a0 register preservation across context switches...\n\n");
    
    // テスト1: 基本的なコンテキストスイッチテスト
    printf("Test 1: Basic context switch\n");
    uthread_add2(thread1, 12345, stack1, STACK_SIZE);
    uthread_add2(thread2, 67890, stack2, STACK_SIZE);
    uthread_start();
    
    if (test_passed) {
        printf("\nBasic test PASSED\n");
    } else {
        printf("\nBasic test FAILED\n");
    }
    
    // テスト2: 複数回のyieldテスト
    printf("\nTest 2: Multiple yields\n");
    uthread_add2(thread3, 55555, stack1, STACK_SIZE);
    uthread_start();
    
    printf("\n=== Test Complete ===\n");
    exit(0);
}