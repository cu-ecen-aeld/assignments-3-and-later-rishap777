ARCH?=aarch64
arc := $(shell arch)
ifeq ($(ARCH), aarch64)
	cc=/__w/assignment-4-rishap777/assignment-4-rishap777/buildroot/output/host/bin/aarch64-buildroot-linux-gnu-gcc
else
	cc=gcc
endif


all: aesdsocket.o

default: aesdsocket.o

aesdsocket.o: aesdsocket.c
	echo $(arc)
	$(cc) aesdsocket.c -o aesdsocket

clean:
	rm -f aesdsocket
	

