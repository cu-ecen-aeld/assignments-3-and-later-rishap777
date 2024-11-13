ARCH=AArch64

cc=$(CROSS_COMPILE)gcc

all: aesdsocket.o

default: aesdsocket.o

aesdsocket.o: aesdsocket.c
	$(cc) aesdsocket.c -o aesdsocket

clean:
	rm -f aesdsocket
	

