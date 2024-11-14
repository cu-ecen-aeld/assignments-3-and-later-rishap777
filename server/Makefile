ARCH?=aarch64

ifeq ($(ARCH), aarch64)
	cc=aarch64-none-linux-gnu-gcc
else
	cc=gcc
endif


all: aesdsocket.o

default: aesdsocket.o

aesdsocket.o: aesdsocket.c
	$(cc) aesdsocket.c -o aesdsocket

clean:
	rm -f aesdsocket
	

