#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int fd, n;
  char buf[128];

  if(argc != 2){
    fprintf(2, "Usage: chatrecv <fifo_name>\n");
    exit(1);
  }

  fd = open(argv[1], O_RDONLY);
  if(fd < 0){
    fprintf(2, "chatrecv: cannot open %s\n", argv[1]);
    exit(1);
  }


  int pid = getpid();
  while((n = read(fd, buf, sizeof(buf) - 1)) > 0){
    buf[n] = 0;  // null terminate
    printf("[received pid=%d] %s", pid, buf);
  }

  close(fd);
  exit(0);
}
