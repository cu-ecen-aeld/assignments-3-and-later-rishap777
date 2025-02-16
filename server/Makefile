CC=/_work/assignment-6-rishap777/assignment-6-rishap777/build/tmp/sysroots-components/x86_64/gcc-cross-aarch64/usr/bin/aarch64-poky-linux/aarch64-poky-linux-gcc

#CFLAGS ?= -g -Wall -Werror

#TARGET ?= aesdsocket

#LDFLAGS ?= -lpthread -lrt

all:aesdsocket
default:aesdsocket
aesdsocket:aesdsocket.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	
clean:
	rm -f aesdsocket
	

