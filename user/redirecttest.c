#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

void catfile(char *filename)
{
  int fd;
  char buf[64];
  int n;

  fd = open(filename, O_RDONLY);
  if(fd < 0){
    fprintf(2, "cat: cannot open %s\n", filename);
    return;
  }
  while((n = read(fd, buf, sizeof(buf) - 1)) > 0){
    buf[n] = 0;
    printf("%s", buf);
  }
  close(fd);
}

int main(int argc, char *argv[])
{
  int fd;

  // テスト用ファイルを削除（存在する場合）
  unlink("rtest.txt");

  printf("=== Shell >> redirection test ===\n\n");

  // 1. > でファイル作成 (O_WRONLY|O_CREATE|O_TRUNC)
  printf("$ echo Hello > rtest.txt\n");
  fd = open("rtest.txt", O_WRONLY | O_CREATE | O_TRUNC);
  if(fd < 0){
    fprintf(2, "open failed\n");
    exit(1);
  }
  write(fd, "Hello\n", 6);
  close(fd);

  printf("$ cat rtest.txt\n");
  catfile("rtest.txt");
  printf("\n");

  // 2. >> でファイルに追記 (O_WRONLY|O_CREATE|O_APPEND)
  printf("$ echo World >> rtest.txt\n");
  fd = open("rtest.txt", O_WRONLY | O_CREATE | O_APPEND);
  if(fd < 0){
    fprintf(2, "open O_APPEND failed\n");
    exit(1);
  }
  write(fd, "World\n", 6);
  close(fd);

  printf("$ cat rtest.txt\n");
  catfile("rtest.txt");
  printf("\n");

  // 3. もう一度 >> で追記
  printf("$ echo xv6 >> rtest.txt\n");
  fd = open("rtest.txt", O_WRONLY | O_CREATE | O_APPEND);
  if(fd < 0){
    fprintf(2, "open O_APPEND failed\n");
    exit(1);
  }
  write(fd, "xv6\n", 4);
  close(fd);

  printf("$ cat rtest.txt\n");
  catfile("rtest.txt");
  printf("\n");

  // 4. > で上書き (O_TRUNCでファイル内容がクリアされる)
  printf("$ echo OVERWRITE > rtest.txt\n");
  fd = open("rtest.txt", O_WRONLY | O_CREATE | O_TRUNC);
  if(fd < 0){
    fprintf(2, "open O_TRUNC failed\n");
    exit(1);
  }
  write(fd, "OVERWRITE\n", 10);
  close(fd);

  printf("$ cat rtest.txt\n");
  catfile("rtest.txt");

  exit(0);
}
