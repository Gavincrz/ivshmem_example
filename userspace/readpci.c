#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAP_SIZE 4096UL


void print_error() {
  fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", \
  __LINE__, __FILE__, errno, strerror(errno));
   exit(1);
}

int main(int argc, char **argv) {
  int fd;
  char *filename;
  char *map_base;

  if (argc < 2) {
    fprintf(stderr, "need specify file path to read");
    exit(1);
  }

  filename = argv[1];
  if ((fd = open(filename, O_RDWR | O_SYNC)) == -1) print_error();
  printf("%s opened.\n", filename);

  map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if(map_base == (void *) -1) print_error();

  printf("PCI Memory mapped to address 0x%08lx.\n", (unsigned long) map_base);

  for (int i = 0; i < 10; i++){
    printf("0x%02x ", map_base[i]);
  }

  close(fd);
  return 0;

}
