ARCH?=x86_64
arc := $(shell arch)
ifeq ($(ARCH), aarch64)
	cc=$(CROSS_COMPILE)
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
	

