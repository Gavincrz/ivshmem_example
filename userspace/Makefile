CC=gcc
CFLAGS = -lrt

all: readpci trigger
readpci: rwpci.c
	$(CC) -o rwpci rwpci.c $(CFLAGS)
trigger:
	$(CC) -o trigger trigger.c $(CFLAGS)
clean:
	rm -f rwpci trigger
