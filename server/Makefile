ARCH?=aarch64
arc := $(shell arch)
echo arc
ifeq ($(ARCH), aarch64)
	echo "$arc"
	cc=/__w/assignment-4-rishap777/assignment-4-rishap777/buildroot/output/host/bin/aarch64-buildroot-linux-gnu-gcc
else
	echo "$arc"
	cc=gcc
endif


all: aesdsocket.o

default: aesdsocket.o

aesdsocket.o: aesdsocket.c
	$(cc) aesdsocket.c -o aesdsocket

clean:
	rm -f aesdsocket
	

