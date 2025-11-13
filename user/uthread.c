#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/uthread.h"

// Saved registers for kernel context switches. (from kernel/proc.h)
struct context
{
  uint64 ra;
  uint64 sp;

  // callee-saved
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
};

// swtch.S (from kernel/defs.h)
void swtch(struct context *, struct context *);

// xv6のプロセスの状態
enum
{
  UNUSED,
  USED,
  RUNNABLE,
  RUNNING,
  EXITED
};

struct uthread
{
  int tid;
  int state;
  void (*func)(void);
  struct context context;
  uint8 *stack;
  uint64 stack_size;
};

// スレッドテーブルの実装
struct uthread uthreads[MAX_UTHREADS];
int next_tid = 1;
int current_tid = -1;

struct context scheduler_context;

static struct uthread *alloc_thread(void)
{
  struct uthread *t;

  for (t = uthreads; t < &uthreads[MAX_UTHREADS]; t++)
  {
    if (t->state == UNUSED)
    {
      t->tid = next_tid++;
      t->state = USED;
      return t;
    }
  }
  return 0;
}

static void thread_wrapper(void)
{
  struct uthread *t = &uthreads[current_tid - 1];
  t->func();
  uthread_exit();
}

int uthread_add(void (*f)(void), uint8 *stack, uint64 size)
{
  struct uthread *t;

  t = alloc_thread();
  if (t == 0)
    return -1;

  t->func = f;
  t->stack = stack;
  t->stack_size = size;
  t->state = RUNNABLE;

  memset(&t->context, 0, sizeof(t->context)); // コンテキストの初期化
  t->context.ra = (uint64)thread_wrapper;     // リターンアドレスの設定
  t->context.sp = (uint64)(stack + size);     // スタックポインタの設定

  return t->tid;
}

void uthread_start(void)
{
  struct uthread *t;
  int found;
  for (;;)
  {
    found = 0;

    for (t = uthreads; t < &uthreads[MAX_UTHREADS]; t++)
    {
      if (t->state == RUNNABLE)
      {
        found = 1;

        t->state = RUNNING;
        current_tid = t->tid;
        swtch(&scheduler_context, &t->context);

        current_tid = -1;

        if (t->state == EXITED)
        {
          t->state = UNUSED;
        }
      }
    }

    if (!found)
      break;
  }
}

void uthread_yield(void)
{
  struct uthread *t = &uthreads[current_tid - 1];

  t->state = RUNNABLE;
  swtch(&t->context, &scheduler_context);
}

void uthread_exit(void)
{
  struct uthread *t = &uthreads[current_tid - 1];

  t->state = EXITED;
  swtch(&t->context, &scheduler_context);
}

int uthread_gettid(void)
{
  return current_tid;
}

void uthread_acquire(int *locked)
{
  for (;;)
  {
    if (*locked == 0)
    {
      *locked = 1;
      break;
    }
    uthread_yield();
  }
}

void uthread_release(int *locked)
{
  *locked = 0;
}
