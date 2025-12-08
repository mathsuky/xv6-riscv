#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int fd;
  char buf[64];
  int n;

  fd = open("test.txt", O_WRONLY | O_CREATE | O_TRUNC);
  if(fd < 0){
    fprintf(2, "open failed\n");
    exit(1);
  }
  write(fd, "Hello\n", 6);
  close(fd);

  fd = open("test.txt", O_WRONLY | O_APPEND);
  if(fd < 0){
    fprintf(2, "open O_APPEND failed\n");
    exit(1);
  }
  write(fd, "World\n", 6);
  close(fd);

  fd = open("test.txt", O_RDONLY);
  if(fd < 0){
    fprintf(2, "open for read failed\n");
    exit(1);
  }
  printf("Result:\n");
  while((n = read(fd, buf, sizeof(buf) - 1)) > 0){
    buf[n] = 0;
    printf("%s", buf);
  }
  close(fd);

  exit(0);
}
