ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-
cc=$(CROSS_COMPILE)gcc

all: aesdsocket.o

default: aesdsocket.o

aesdsocket.o: aesdsocket.c
	$(cc) aesdsocket.c -o aesdsocket

clean:
	rm -f aesdsocket
	

