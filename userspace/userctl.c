#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#define CMD_READ_SHMEM 0
#define CMD_READ_VMID 1
#define CMD_INTERRUPT 2

void get_sharemem(int fd){
  int value;
  if (ioctl(fd, CMD_READ_SHMEM, &value) == -1)
  {
    perror("failed to get sharemem \n");
  }
  else
  {
    printf("Status : Successfully get sharemem, %s\n", (char *)&value);
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

void send_interrupt(int fd, int dest_vm){
  if (ioctl(fd, CMD_INTERRUPT, &dest_vm) == -1)
  {
    perror("failed to write to doorbell\n");
  }
  else
  {
    printf("Shuang dao 1 \n");
  }
}

int main(int argc, char *argv[])
{
  char *file_name = "/dev/ivshmem";
  int fd;

  int option;
  int dest_vm = 0;

  // parsing arguments
  if (argc == 1){
    option = CMD_READ_SHMEM;
  }
  else{
    if (strcmp(argv[1], "-m") == 0){
      option = CMD_READ_SHMEM;
    }
    else if (strcmp(argv[1], "-d") == 0){
      option = CMD_READ_VMID;
    }
    else if ((strcmp(argv[1], "-i") == 0) && (argc == 3)){
      option = CMD_INTERRUPT;
      dest_vm = atoi(argv[2]);
      printf("destination vm id is %d \n", dest_vm);
    }
    else {
      printf("Usage: \n");
      printf("sudo ./userctl -m                // get shmem content\n");
      printf("sudo ./userctl -d                // get self vm id\n");
      printf("sudo ./userctl -i <dest_vm>      // trigger interrupt to dest_vm\n");
    }
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
    case CMD_INTERRUPT:
      send_interrupt(fd, dest_vm);
      break;
    default:
      break;
  }
  close(fd);

}
