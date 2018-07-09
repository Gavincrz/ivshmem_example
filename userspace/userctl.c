#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#define CMD_READ_SHMEM 0
#define CMD_FAKE1 1

void get_sharemem(int fd){
  int tmp;
  if (ioctl(fd, CMD_READ_SHMEM, &tmp) == -1)
  {
    perror("failed to get sharemem");
  }
  else
  {
    printf("Status : Successfully get sharemem");
  }
}

void fake_cmd1(int fd){
  int tmp;
  if (ioctl(fd, CMD_FAKE1, &tmp) == -1)
  {
    perror("failed to get fake_cmd1");
  }
  else
  {
    printf("Status : Successfully fake_cmd1");
  }
}

int main(int argc, char *argv[])
{
  char *file_name = "/dev/ivshmem";
  int fd;

  enum{
    read_share,
    fake1,
    fake2
  } option;

  if (argc == 1){
    option = read_share;
  }
  else if (argc == 2){
    option = fake1;
  }
  fd = open(file_name, O_RDWR);
  if (fd == -1)
  {
    perror("open file failed");
    return 2;
  }

  switch (option)
  {
    case read_share:
      get_sharemem(fd);
      break;
    case fake1:
      fake_cmd1(fd);
      break;
    default:
      break;
  }
  close(fd);

}
