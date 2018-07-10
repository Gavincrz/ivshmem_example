#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include "../ivshmem_common.h"


void get_sharemem(int fd){
  int value;
  if (ioctl(fd, CMD_READ_SHMEM, &value) == -1)
  {
    perror("failed to get sharemem \n");
  }
  else
  {
    printf("Status : Successfully get sharemem, 0x%x\n", value);
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

void send_interrupt(int fd, int dest_vm, int msg){
  irq_arg arg;
  arg.dest_id = dest_vm;
  arg.msg = msg;

  if (ioctl(fd, CMD_INTERRUPT, &arg) != 0)
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

  // print something to understand
  printf("command %ld, %ld, %ld\n",CMD_READ_SHMEM, CMD_READ_VMID, CMD_INTERRUPT);

  char *file_name = "/dev/ivshmem";
  int fd, msg, option;
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
    else if ((strcmp(argv[1], "-i") == 0) && (argc == 4)){
      option = CMD_INTERRUPT;
      dest_vm = atoi(argv[2]);
      msg = atoi(argv[3]);
      printf("destination vm id is %d \n", dest_vm);
    }
    else {
      printf("Usage: \n");
      printf("sudo ./userctl -m                     // get shmem content\n");
      printf("sudo ./userctl -d                     // get self vm id\n");
      printf("sudo ./userctl -i <dest_vm> <msg>     // trigger interrupt to dest_vm\n");
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
      send_interrupt(fd, dest_vm, msg);
      break;
    default:
      break;
  }
  close(fd);

}
