#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#define CMD_READ_SHMEM 0
#define CMD_READ_VMID 1

void get_sharemem(int fd){
  char *value;
  if (ioctl(fd, CMD_READ_SHMEM, value) == -1)
  {
    perror("failed to get sharemem\n");
  }
  else
  {
    printf("Status : Successfully get sharemem\n");
  }
}

void get_vmid(int fd){
  int vmid;
  if (ioctl(fd, CMD_READ_VMID, &vmid) == -1)
  {
    perror("failed to get vmid\n");
  }
  else
  {
    printf("Status : vmid is %d\n", vmid);
  }
}

int main(int argc, char *argv[])
{
  char *file_name = "/dev/ivshmem";
  int fd;

  int option;

  if (argc == 1){
    option = CMD_READ_SHMEM;
  }
  else if (argc == 2){
    option = CMD_READ_VMID;
  }
  fd = open(file_name, O_RDWR);
  if (fd == -1)
  {
    perror("open file failed\n");
    return 2;
  }

  switch (option)
  {
    case CMD_READ_SHMEM:
      get_sharemem(fd);
      break;
    case CMD_READ_VMID:
      get_vmid(fd);
      break;
    default:
      break;
  }
  close(fd);

}
