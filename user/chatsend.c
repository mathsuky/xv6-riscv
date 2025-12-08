#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int fd;
  char buf[128];

  if(argc != 2){
    fprintf(2, "Usage: chatsend <fifo_name>\n");
    exit(1);
  }

  fd = open(argv[1], O_WRONLY);
  if(fd < 0){
    fprintf(2, "chatsend: cannot open %s\n", argv[1]);
    exit(1);
  }

  printf("Type messages:\n");

  int pid = getpid();
  while(gets(buf, sizeof(buf))){
    if(buf[0] == 0) break;  // EOF
    int len = strlen(buf);
    if(len > 0 && buf[len-1] == '\n') buf[len-1] = 0;
    fprintf(fd, "[from pid=%d] %s\n", pid, buf);
  }

  close(fd);
  exit(0);
}
